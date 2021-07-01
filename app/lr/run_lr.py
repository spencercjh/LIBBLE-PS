#!usr/bin/python

import os

# TODO use yaml or json instead of py code here
# --------------------------------------
# modify these arguments to run the program
machine_file = ' /home/spencercjh/CLionProjects/parameter-server/LIBBLE-PS/host_file '  # hosts file
test_data_file = ' null '  # test data path, 'null': no test_file
train_data_file = ' /home/spencercjh/codes/dataset/webspam_wc_normalized_trigram/webspam_wc_normalized_trigram.svm '  # train data path.
partition_directory = "/home/spencercjh/codes/dataset/webspam_wc_normalized_trigram/8"
n_cols = ' 16609143 '  # train data feature number
n_rows = ' 350000 '  # train data instance number
n_servers = ' 2 '
n_workers = ' 8 '
n_epoches = ' 1 '
n_iters = ' 10 '
rate = ' 1 '  # step size
lam = ' 0.0001 '  # regularization hyperparameter
param_init = ' 0 '  # parameter initialization. 0--all zero 1--randomize to [0,1]
# --------------------------------------

n_trainers = str(1 + int(n_servers) + int(n_workers))

# TODO pretty exec format
command = 'mpirun -n ' + n_trainers + ' -f ' + machine_file + ' ./lr ' + ' -n_servers ' + n_servers + '-n_workers ' + n_workers + ' -n_epoches' + n_epoches + ' -n_iters' + n_iters + ' -n_cols' + n_cols + ' -n_rows' + n_rows + ' -test_data_file ' + test_data_file + ' -train_data_file ' + train_data_file + ' -partition_directory ' + partition_directory + ' -rate ' + rate + ' -lambda ' + lam + ' -param_init ' + param_init
os.system(command)
