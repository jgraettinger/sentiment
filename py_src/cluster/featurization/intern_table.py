
class InternTable(object):

    def __init__(self):
        self._str_to_id = {}
        self._id_to_str = {}
        self._id_to_cnt = {}
        self._next_id = 0
        return

    def add_token(self, str):

        if str not in self._str_to_id:
            self._str_to_id[str] = self._next_id
            self._id_to_cnt[self._next_id] = 0
            self._id_to_str[self._next_id] = str
            self._next_id += 1

        return self._str_to_id[str]

    def get_id(self, str):
        return self._str_to_id[str]

    def get_str(self, id):
        return self._id_to_str[id]

    def add_reference(self, id):
        self._id_to_cnt[id] += 1

    def drop_reference(self, id):

        if self._id_to_cnt[id] > 1:
            self._id_to_cnt[id] -= 1
            return

        str = self._id_to_str[id]
        del self._str_to_id[str]
        del self._id_to_str[id]
        del self._id_to_cnt[id]

    def get_reference_count(self, id):
        return self._id_to_cnt[id]

