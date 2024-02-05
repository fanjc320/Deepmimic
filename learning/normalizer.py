import numpy as np
import copy
import util.mpi_util as MPIUtil
from util.logger import Logger

class Normalizer(object):
    CHECK_SYNC_COUNT = 50000 # check synchronization after a certain number of entries

    # these group IDs must be the same as those in CharController.h
    NORM_GROUP_SINGLE = 0
    NORM_GROUP_NONE = -1

    class Group(object):
        def __init__(self, id, indices):
            self.id = id
            self.indices = indices
            return

    def __init__(self, size, groups_ids=None, eps=0.02, clip=np.inf):
        self.eps = eps
        self.clip = clip
        self.mean = np.zeros(size)
        self.mean_sq = np.zeros(size)
        self.std = np.ones(size)
        self.count = 0
        self.groups = self._build_groups(groups_ids)

        self.new_count = 0
        self.new_sum = np.zeros_like(self.mean)
        self.new_sum_sq = np.zeros_like(self.mean_sq)
        return

    def record(self, x):
        # x = {ndarray: (14, 197)}[
        #     [9.94935710e-01  8.84977356e-01  6.22163681e-03...  6.35215340e-01, 4.67774059e+00 - 7.94864161e+00], [
        #         2.48281949e-03  8.72950969e-01 - 1.81474826e-03...  7.23735399e-01, 4.48241944e-01 - 2.27346621e+00], [
        #         1.00299289e-02  8.44439816e-01 - 4.04141510e
        #         array = {
        #                     NdArrayItemsContainer} < pydevd_plugins.extensions.types.pydevd_plugin_numpy_types.NdArrayItemsContainer
        # object
        # at
        # 0x000001799B48E438 >
        # dtype = {dtype: 0}
        # float64
        # max = {float64}
        # 19.670763372495387
        # min = {float64} - 17.719782374323994
        # shape = {tuple: 2}(14, 197)
        # size = {int}
        # 2758

        size = self.get_size()
        is_array = isinstance(x, np.ndarray)
        if not is_array:
            assert(size == 1)
            x = np.array([[x]])

        assert x.shape[-1] == size, \
            Logger.print('Normalizer shape mismatch, expecting size {:d}, but got {:d}'.format(size, x.shape[-1]))
        x = np.reshape(x, [-1, size])

        self.new_count += x.shape[0] # 27（14,197）
        self.new_sum += np.sum(x, axis=0) # （197，）
        # max = {float64}
        # 105.13273137832756
        # min = {float64} - 112.60357648939559
        # shape = {tuple: 1}
        # 197
        # size = {int}
        # 197

        self.new_sum_sq += np.sum(np.square(x), axis=0) #(197,)
        return

    def update(self):
        new_count = MPIUtil.reduce_sum(self.new_count)
        new_sum = MPIUtil.reduce_sum(self.new_sum) #(197,)
        new_sum_sq = MPIUtil.reduce_sum(self.new_sum_sq) #(197,)

        new_total = self.count + new_count
        if (self.count // self.CHECK_SYNC_COUNT != new_total // self.CHECK_SYNC_COUNT):
            assert self.check_synced(), Logger.print('Normalizer parameters desynchronized')

        if new_count > 0:
            new_mean = self._process_group_data(new_sum / new_count, self.mean)
            new_mean_sq = self._process_group_data(new_sum_sq / new_count, self.mean_sq)
            w_old = float(self.count) / new_total
            w_new = float(new_count) / new_total

            self.mean = w_old * self.mean + w_new * new_mean #(197,)
            self.mean_sq = w_old * self.mean_sq + w_new * new_mean_sq
            self.count = new_total
            self.std = self.calc_std(self.mean, self.mean_sq) #(197)

            self.new_count = 0
            self.new_sum.fill(0)
            self.new_sum_sq.fill(0)

        return

    def get_size(self):
        return self.mean.size

    def set_mean_std(self, mean, std):
        size = self.get_size()
        is_array = isinstance(mean, np.ndarray) and isinstance(std, np.ndarray)
        
        if not is_array:
            assert(size == 1)
            mean = np.array([mean])
            std = np.array([std])

        assert len(mean) == size and len(std) == size, \
            Logger.print('Normalizer shape mismatch, expecting size {:d}, but got {:d} and {:d}'.format(size, len(mean), len(std)))
        
        self.mean = mean
        self.std = std
        self.mean_sq = self.calc_mean_sq(self.mean, self.std)
        return

    def normalize(self, x):
        norm_x = (x - self.mean) / self.std
        norm_x = np.clip(norm_x, -self.clip, self.clip)
        return norm_x

    def unnormalize(self, norm_x):
        x = norm_x * self.std + self.mean
        return x

    def calc_std(self, mean, mean_sq):
        var = mean_sq - np.square(mean)
        # some time floating point errors can lead to small negative numbers
        var = np.maximum(var, 0)
        std = np.sqrt(var)
        std = np.maximum(std, self.eps)
        return std

    def calc_mean_sq(self, mean, std):
        return np.square(std) + np.square(self.mean)

    def check_synced(self):
        synced = True
        if MPIUtil.is_root_proc():
            vars = np.concatenate([self.mean, self.mean_sq])
            MPIUtil.bcast(vars)
        else:
            vars_local = np.concatenate([self.mean, self.mean_sq])
            vars_root = np.empty_like(vars_local)
            MPIUtil.bcast(vars_root)
            synced = (vars_local == vars_root).all()
        return synced

    def _build_groups(self, groups_ids):
        groups = []
        if groups_ids is None:
            curr_id = self.NORM_GROUP_SINGLE
            curr_list = np.arange(self.get_size()).astype(np.int32)
            groups.append(self.Group(curr_id, curr_list))
        else:
            ids = np.unique(groups_ids)
            for id in ids:
                curr_list = np.nonzero(groups_ids == id)[0].astype(np.int32)
                groups.append(self.Group(id, curr_list))

        return groups

    def _process_group_data(self, new_data, old_data):
        proc_data = new_data.copy()
        for group in self.groups:
            if group.id == self.NORM_GROUP_NONE:
                proc_data[group.indices] = old_data[group.indices]
            elif group.id != self.NORM_GROUP_SINGLE:
                avg = np.mean(new_data[group.indices])
                proc_data[group.indices] = avg
        return proc_data