
import re
import simplejson
import sqlalchemy.orm
import formencode
import formencode.validators as fva

from base import BaseModel

from eventlet.green import subprocess

COMMA_RE = re.compile('\s*(?:,\s*)+')

class Regression(BaseModel):

    # collection of active _LiveRegression instances
    _live_instances = {}

    class Schema(formencode.Schema):
        id = fva.Int(if_missing = None)
        name = fva.String(strip = True, not_empty = True)
        regression_database = fva.String(not_empty = True)
        clusterer = fva.String(strip = True, not_empty = True)
        feature_transform = fva.String(strip = True)
        iteration_count = fva.Int(default = 100, min = 1, max = 200)
        vinz_version = fva.String(strip = True, default = 'Vinz')
        class_names = fva.String(not_empty = True)
        config_overrides = fva.String(default = '{}')
        expected_results = fva.String(default = '{}')

    @classmethod
    def validate_common(cls, args):

        if 'class_names' in args:
            assert len(set(COMMA_RE.split(args['class_names']))) >= 2, \
                "At least two class-names must be provided"

        if 'config_overrides' in args:
            try:
                assert isinstance(simplejson.loads(
                    args['config_overrides']), dict)
            except:
                raise AssertionError("Config-overrides must be json dictionary")

        if 'expected_results' in args:
            try:
                assert isinstance(simplejson.loads(
                    args['expected_results']), dict)
            except:
                raise AssertionError("Expected-results must be json dictionary")

        return args

    # property view on class_names_list, returning a comma-seperated string
    def _get_class_names(self):
        return ', '.join(self.class_names_list)
    def _set_class_names(self, class_names):
        self.class_names_list = sorted(COMMA_RE.split(class_names))
        self._orm_class_names = self._get_class_names()
    class_names = property(_get_class_names, _set_class_names)

    # property view on config_overrides_dict, returning json
    def _get_conf_overrides(self):
        return simplejson.dumps(self.config_overrides_dict)
    def _set_conf_overrides(self, conf_overrides):
        self.config_overrides_dict = simplejson.loads(conf_overrides)
        self._orm_config_overrides = self._get_conf_overrides()
    config_overrides = property(_get_conf_overrides, _set_conf_overrides)

    # property view on expected_results_dict, returning json
    def _get_expected_results(self):
        return simplejson.dumps(self.expected_results_dict)
    def _set_expected_results(self, exp_results):
        self.expected_results_dict = simplejson.loads(exp_results)
        self._orm_expected_results = self._get_expected_results()
    expected_results = property(_get_expected_results, _set_expected_results)

    def __init__(self, **kwargs):
        BaseModel.__init__(self, **kwargs)
        self._reconstruct()
        return

    @sqlalchemy.orm.reconstructor 
    def _reconstruct(self):
        if getattr(self, '_orm_class_names'):
            self.class_names = self._orm_class_names
        if getattr(self, '_orm_config_overrides'):
            self.config_overrides = self._orm_config_overrides
        if getattr(self, '_orm_expected_results'):
            self.expected_results = self._orm_expected_results

        if self.id in self._live_instances:
            self._live = self._live_instances[self.id]
        else:
            self._live = self._live_instances.setdefault(
                self.id, _LiveRegression())
        return

    def run(self):
        return self._live.run(self)

    def regression_results(self):

        results = []
        # is currently running, or hasn't yet run
        if self.is_running or not self.precision:
            return results

        exp_prec = self.expected_results_dict.get('precision', {})
        exp_recall = self.expected_results_dict.get('recall', {})

        for class_name in self.class_names_list:
            p = round(self.precision[class_name][-1], 4)
            r = round(self.recall[class_name][-1], 4)

            e_p = exp_prec.get(class_name, None)
            e_r = exp_recall.get(class_name, None)
            e_p = round(e_p, 4) if e_p else None
            e_r = round(e_r, 4) if e_r else None

            if e_p != None:
                results.append(('%s-precision' % class_name, e_p, p))
            if e_r != None:
                results.append(('%s-recall' % class_name, e_r, r))
            if e_p != None and e_r != None:
                f = 2.0 * p * r / (p + r)
                e_f = 2.0 * e_p * e_r / (e_p + e_r)
                results.append(('%s-fscore' % class_name, e_f, f))

        return results

    @property
    def is_running(self):
        return self._live.is_running

    @property
    def precision(self):
        return self._live.precision

    @property
    def recall(self):
        return self._live.recall

    @property
    def entropy(self):
        return self._live.entropy

    @property
    def prior_prob(self):
        return self._live.prior_prob

    @classmethod
    def define_orm_mapping(kls, orm):
        import sqlalchemy as _

        tab = _.Table('regression', orm.meta,
            _.Column('id', _.Integer, primary_key = True),
            _.Column('name', _.String,
                index = True, unique = True, nullable = False),
            _.Column('regression_database', _.String, nullable = False),
            _.Column('clusterer', _.String, nullable = False),
            _.Column('feature_transform', _.String),
            _.Column('iteration_count', _.Integer, nullable = False),
            _.Column('vinz_version', _.String, nullable = False),
            _.Column('class_names', _.String, nullable = False),
            _.Column('config_overrides', _.String),
            _.Column('expected_results', _.String),
        )
        orm.cluster = sqlalchemy.orm.mapper(kls, tab, properties = {
            'class_names': sqlalchemy.orm.synonym(
                '_orm_class_names', map_column = True),
            'config_overrides': sqlalchemy.orm.synonym(
                '_orm_config_overrides', map_column = True),
            'expected_results': sqlalchemy.orm.synonym(
                '_orm_expected_results', map_column = True)
        })


class _LiveRegression(object):

    def __init__(self):
        self.precision = {}
        self.recall = {}
        self._proc = None
        return

    @property
    def is_running(self):
        return self._proc is not None

    def run(self, reg_model):

        if self.is_running:
            raise RuntimeError("Regression is already running!")

        self.entropy = []
        self.precision = {}
        self.recall = {}
        self.prior_prob = {}

        self._proc = subprocess.Popen(['python'],
            stdin  = subprocess.PIPE,
            stdout = subprocess.PIPE,
            stderr = subprocess.PIPE)

        cur_script = """
        from pkg_resources import load_entry_point

        load_entry_point(
            %(vinz_version)r, "vinz", "run_regression"
        )(
            %(iteration_count)r,
            %(regression_database)r,
            %(config_overrides_dict)r,
            %(clusterer)r,
            %(feature_transform)r,
            %(class_names_list)r,
        )
        """ % reg_model.__dict__

        cur_script = '\n'.join(i[8:] for i in cur_script.split('\n'))

        print cur_script
        self._proc.stdin.write(cur_script)
        self._proc.stdin.close()

        for iter_no, line in enumerate(self._proc.stdout):
            entropy, precision, recall, prior_prob = simplejson.loads(line)

            self.entropy.append(entropy)
            for k, v in precision.items():
                self.precision.setdefault(k, []).append(v)
            for k, v in recall.items():
                self.recall.setdefault(k, []).append(v)
            for k, v in prior_prob.items():
                self.prior_prob.setdefault(k, []).append(v)

        err = self._proc.stderr.read()
        code = self._proc.wait()
        self._proc = None

        if code:
            raise RuntimeError("Regression exited with code:\n%s" % err)

        return

