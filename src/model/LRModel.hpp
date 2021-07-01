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

#ifndef _LRMODEL_HPP_
#define _LRMODEL_HPP_

#include <cmath>

#include "../storage/include_storage.hpp"
#include "Model.hpp"

class LRModel : public Model {
public:
    LRModel() {}
    double compute_loss(const DataSet &ds, const Parameter &params, const int num_of_all_data,
                        const int num_workers, const double lambda) {
        double loss = 0;
        for (int i = 0; i < ds.num_rows; i++) {
            DataPoint &d = ds.data[i];
            double z = 0;
            for (int j = 0; j < d.key.size(); j++) {
                z += params.parameter[d.key[j]] * d.value[j];
            }
            loss += log(1 + exp(-d.label * z));
        }
        loss /= (double) num_of_all_data;
        double index = 0.5 * lambda / ((double) num_workers);
        for (int i = 0; i < params.parameter.size(); i++) {
            loss += index * pow(params.parameter[i], 2);
        }
        return loss;
    }

    void compute_full_gradient(const DataSet &ds, const Parameter &params, Gradient_Dense &g,
                               const int num_of_all_data) {
        g.reset();
        for (int i = 0; i < ds.num_rows; i++) {
            DataPoint &d = ds.data[i];
            double z = 0;
            for (int j = 0; j < d.key.size(); j++) {
                z += params.parameter[d.key[j]] * d.value[j];
            }
            z = -d.label * (1 - 1 / (1 + exp(-d.label * z))) / num_of_all_data;
            for (int j = 0; j < d.key.size(); j++) {
                g.gradient[d.key[j]] += z * d.value[j];
            }
        }
    }

    void update(const DataSet &ds, Parameter &params,
                const Gradient_Dense &full_grad, const double lambda,
                const int num_epoches, const double rate, const int recover_index,
                const int num_of_all_data, const int num_workers) {
        const std::vector<double> old_params = params.parameter;
        double a = 1, b = 0;
        srand(time(NULL));
        for (int i = 0; i < num_epoches * (num_of_all_data / num_workers); i++) {
            if (recover_index != 0 && i % recover_index == 0) {
                vector_multi_add(params.parameter, a, full_grad.gradient, b);
                a = 1;
                b = 0;
            }
            double z, z1 = 0, z2 = 0;
            const DataPoint &d = ds.data[rand() % (ds.num_rows - 1)];
            for (int j = 0; j < d.key.size(); j++) {
                z1 += (a * params.parameter[d.key[j]] + b * full_grad.gradient[d.key[j]]) *
                      d.value[j];
                z2 += old_params[d.key[j]] * d.value[j];
            }
            b = (1 - lambda * rate) * b - rate;
            a = (1 - lambda * rate) * a;
            z = rate * d.label * (1 / (1 + exp(-d.label * z1)) - 1 / (1 + exp(-d.label * z2))) / a;
            for (int j = 0; j < d.key.size(); j++) {
                params.parameter[d.key[j]] -= z * d.value[j];
            }
        }
    }
};

#endif