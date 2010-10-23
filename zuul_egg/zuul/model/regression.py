
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
        if self.is_running or not self.has_results:
            return results

        for stat_name, exp_stat_val in self.expected_results_dict.items():

            assert hasattr(self, stat_name), "No such statistic"

            act_stat_val = getattr(self, stat_name)

            if isinstance(act_stat_val, dict):
                # series per-class
                for class_name in self.class_names_list:
                    exp = round(exp_stat_val[class_name], 4)
                    act = round(act_stat_val[class_name][-1], 4)
                    results.append(('%s-%s' % (class_name, stat_name), exp, act))

            else:
                # simple series
                exp = round(exp_stat_val, 4)
                act = round(act_stat_val[-1], 4)
                results.append(('%s' % stat_name, exp, act))

        return results

    @property
    def is_running(self):
        return self._live.is_running

    @property
    def has_results(self):
        return True if self._live.stats else False

    @property
    def precision(self):
        return self._live.stats['precision']

    @property
    def recall(self):
        return self._live.stats['recall']

    @property
    def fmeasure(self):

        fm = {}
        for cname in self.class_names_list:
            fm[cname] = [(2.0 * p * r / (p + r + 0.00001)) for (p,r) in zip(
                self.precision[cname], self.recall[cname])]

        return fm

    @property
    def entropy(self):
        return self._live.stats['entropy']

    @property
    def log_likelihood(self):
        return self._live.stats['log_likelihood']

    @property
    def prior_probabilities(self):
        return self._live.stats['prior_probabilities']

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
        self.stats = {}
        self._proc = None
        return

    @property
    def is_running(self):
        return self._proc is not None

    def run(self, reg_model):

        if self.is_running:
            raise RuntimeError("Regression is already running!")

        self.stats = {}

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
            stats = simplejson.loads(line)

            for stat_name, stat_value in stats.items():

                if isinstance(stat_value, dict):
                    sub_dict = self.stats.setdefault(stat_name, {})

                    for sub_name, sub_value in stat_value.items():
                        sub_dict.setdefault(sub_name, []).append(sub_value)
                else:
                    self.stats.setdefault(stat_name, []).append(stat_value)

        err = self._proc.stderr.read()
        code = self._proc.wait()
        self._proc = None

        if code:
            raise RuntimeError("Regression exited with code:\n%s" % err)

        return

