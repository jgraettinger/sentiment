
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
        regression_database = fva.String(not_empty = True)
        clusterer = fva.String(strip = True, not_empty = True)
        feature_transform = fva.String(strip = True)
        iteration_count = fva.Int(default = 100, min = 1, max = 200)
        vinz_version = fva.String(strip = True, default = 'Vinz')
        class_names = fva.String(not_empty = True)
        config_overrides = fva.String(default = '{}')

    @classmethod
    def validate_common(cls, args):

        if 'class_names' in args:
            assert len(set(COMMA_RE.split(args['class_names']))) >= 2, \
                "At least two class-names must be provided"

        if 'config_overrides' in args:
            try:
                conf = simplejson.loads(args['config_overrides'])
                assert isinstance(conf, dict), \
                    "Config-overrides must be a dictionary"
            except:
                raise AssertionError("Config-overrides must be json")

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

        if self.id in self._live_instances:
            self._live = self._live_instances[self.id]
        else:
            self._live = self._live_instances.setdefault(
                self.id, _LiveRegression())
        return

    def run(self):
        return self._live.run(self)

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
            _.Column('regression_database', _.String, nullable = False),
            _.Column('clusterer', _.String, nullable = False),
            _.Column('feature_transform', _.String),
            _.Column('iteration_count', _.Integer, nullable = False),
            _.Column('vinz_version', _.String, nullable = False),
            _.Column('class_names', _.String, nullable = False),
            _.Column('config_overrides', _.String),
        )
        orm.cluster = sqlalchemy.orm.mapper(kls, tab, properties = {
            'class_names': sqlalchemy.orm.synonym(
                '_orm_class_names', map_column = True),
            'config_overrides': sqlalchemy.orm.synonym(
                '_orm_config_overrides', map_column = True)
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

