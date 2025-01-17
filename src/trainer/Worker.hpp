/**
        * Copyright (c) 2017 LIBBLE team supervised by Dr. Wu-Jun LI at Nanjing University.
        * All Rights Reserved.
        * Licensed under the Apache License, Version 2.0 (the "License");
        * you may not use this file except in compliance with the License.
        * You may obtain a copy of the License at
        *
        * http://www.apache.org/licenses/LICENSE-2.0
        *
        * Unless required by applicable law or agreed to in writing, software
        * distributed under the License is distributed on an "AS IS" BASIS,
        * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
        * See the License for the specific language governing permissions and
        * limitations under the License. */

#ifndef _WORKER_HPP_
#define _WORKER_HPP_

#include <cassert>
#include <functional>
#include <random>
#include <vector>

#include "../storage/include_storage.hpp"
#include "../util/include_util.hpp"
#include "Trainer.hpp"

class Worker : public Trainer {
private:
    int worker_id;
    int batch_size;
    double lambda;
    char info;
    double rate;
    double MIN = pow(0.1, 290);
    int recover_index = 0;
    DataSet dataset;
    Parameter params;
    Gradient_Dense grad;
    Gradient_Dense full_grad;
    std::string test_data_file;
    DataSet test_dataset;

public:
    Worker(int n_ser, int n_wor, int n_c, int n_r, int n_e, int n_i, int mode_, std::string f,
           std::string partition_directory, Model *model_p, Comm *comm_p, int proc_id, int b_s,
           double lambda_, double r, std::string t_f)
        : Trainer(n_ser, n_wor, n_c, n_r, n_e, n_i, mode_, f, partition_directory, model_p, comm_p),
          worker_id(proc_id),
          batch_size(b_s),
          lambda(lambda_),
          rate(r),
          test_data_file(t_f) {
        params.resize(num_cols);
        grad.resize(num_cols);
        full_grad.resize(num_cols);
    }

    void work() override {
//        std::cout << "Worker: " << worker_id << " begin read_data()" << std::endl;
        read_data();
//        std::cout << "Worker: " << worker_id << " finish read_data()" << std::endl;

        double check_a = 1;
        for (int i = 0; i < num_epoches * (num_of_all_data / num_workers); i++) {
            check_a *= (1 - rate * lambda);
            if (check_a < MIN) {
                recover_index = i;
                break;
            }
        }
        if (test_data_file != "null" && worker_id == 1) { read_test_data(); }
        std::random_device rd;
        std::default_random_engine e(rd());
        std::uniform_int_distribution<> u(0, dataset.get_num_rows() - 1);

//        std::cout << "Worker: " << worker_id << " begin pull() at first" << std::endl;
        pull();
//        std::cout << "Worker: " << worker_id << " finish pull() at first" << std::endl;

//        std::cout << "Worker: " << worker_id << " begin calculate_loss() at first" << std::endl;
        double i_loss = calculate_loss();
//        std::cout << "Worker: " << worker_id << " finish calculate_loss() at first" << std::endl;

//        std::cout << "Worker: " << worker_id << " begin calculate_loss() at first" << std::endl;
        report_loss(i_loss);
        if (worker_id == 1) {
            report_accuracy();
        }
        for (int i = 0; i < num_iters; i++) {
            MPI_Barrier(MPI_COMM_WORLD);// start
//            std::cout << "Worker: " << worker_id << " begin calculate_part_full_gradient() at iter: " << i << std::endl;
            calculate_part_full_gradient();
//            std::cout << "Worker: " << worker_id << " finish calculate_part_full_gradient() at iter: " << i << std::endl;

//            std::cout << "Worker: " << worker_id << " begin push() at iter: " << i << std::endl;
            push();
//            std::cout << "Worker: " << worker_id << " finish push() at iter: " << i << std::endl;

//            std::cout << "Worker: " << worker_id << " begin pull_full_grad() at iter: " << i << std::endl;
            pull_full_grad();
//            std::cout << "Worker: " << worker_id << " finish pull_full_grad() at iter: " << i << std::endl;

//            std::cout << "Worker: " << worker_id << " begin local_update_sparse(u, e) at iter: " << i << std::endl;
            local_update_sparse(u, e);
//            std::cout << "Worker: " << worker_id << " finish local_update_sparse(u, e) at iter: " << i << std::endl;

//            std::cout << "Worker: " << worker_id << " begin scope_push(); at iter: " << i << std::endl;
            scope_push();
//            std::cout << "Worker: " << worker_id << " finish scope_push() at iter: " << i << std::endl;

//            std::cout << "Worker: " << worker_id << " begin pull() at iter: " << i << std::endl;
            pull();
//            std::cout << "Worker: " << worker_id << " finish pull() at iter: " << i << std::endl;

            MPI_Barrier(MPI_COMM_WORLD);// end
            double loss = calculate_loss();
            report_loss(loss);
            if (worker_id == 1) {
                report_accuracy();
            }
        }

//        std::cout << "worker " << worker_id << " done" << std::endl;
    }

    void sample_data(std::vector<int> &sample_ids) {
        //   assert(sample_ids.size() == 0);
        int num_rows = dataset.get_num_rows(), left = batch_size;
        std::default_random_engine e(std::random_device{}());
        for (int i = 0; i < num_rows; i++) {
            int x = e() % (num_rows - i);
            if (x < left) {
                sample_ids.push_back(i);
                left--;
                if (left == 0) break;
            }
        }
    }

    void pull() { comm_ptr->W_recv_params_from_all_S(params); }

    void pull_full_grad() { comm_ptr->W_recv_full_grad_from_all_S(full_grad); }

    void push() { comm_ptr->W_send_grads_to_all_S(grad); }

    void scope_push() { comm_ptr->W_send_params_to_all_S(params); }

    // read data from files
    void read_data() { dataset.read_from_file(partition_directory, worker_id, num_workers, num_cols); }

    // calculate loss for all data
    double calculate_loss() {
        return model_ptr->compute_loss(dataset, params, num_of_all_data, num_workers, lambda);
    }

    // calculate local full gradient
    void calculate_part_full_gradient() {
        model_ptr->compute_full_gradient(dataset, params, grad, num_of_all_data);
    }

    void local_update_sparse(std::uniform_int_distribution<> &u, std::default_random_engine &e) {
        model_ptr->update(dataset, u, e, params, full_grad, lambda, num_epoches, rate, recover_index, num_of_all_data, num_workers);
    }

    // report loss to coordinator
    void report_loss(double loss) { comm_ptr->W_send_loss_to_C(loss); }

    void read_test_data() { test_dataset.read_from_test_file(test_data_file, num_cols); }

    void report_accuracy() {
        if (test_data_file != "null") {
            double accuracy = 0;
            for (int i = 0; i < test_dataset.num_rows; i++) {
                double result = 0;
                DataPoint &d = test_dataset.data[i];
                for (int j = 0; j < d.key.size(); j++) {
                    result += params.parameter[d.key[j]] * d.value[j];
                }
                if (result * d.label > 0) accuracy++;
                else if (result * d.label == 0) {
                    accuracy += 0.5;
                } else
                    ;
            }
            accuracy /= test_dataset.num_rows;
            comm_ptr->W_send_accuracy_to_C(accuracy);
        } else {
            comm_ptr->W_send_accuracy_to_C(-1);
        }
    }
};

#endif