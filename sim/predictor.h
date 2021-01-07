#include <bitset>
#include <map>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include "utils.h"
#include "bt9.h"
#include "bt9_reader.h"
#include "vector_cut_slice.h"

class  PREDICTOR{
        private:
        public:
            PREDICTOR(){
                cell = new gru_cell[128];
                for(int i = 0;i < 64; i ++){
                    srand(i);
                    Wrx[i] = rand() % 16;                   
                    Wrh[i] = rand() % 16;
                    Wzx[i] = rand() % 16;
                    Wzh[i] = rand() % 16;
                    Whx[i] = rand() % 16;
                    Whh[i] = rand() % 16;
                    Wo[i] = rand() % 16;
                }

                for(int i = 0; i < 128; i++){
                    srand(i);
                    cell[i].iftaken = rand() % 2;
                    cell[i].Yout = rand() % 2;
                    for(int j = 0; j < 64; j++){
                        cell[i].hide_feature[j] = rand() % 4;
                    }
                }
            }

            ~PREDICTOR(){
                delete[] cell;
            }

            struct gru_cell{
                int hide_feature[64]; 
                float Yout;
                uint64_t Xin;
                char iftaken;                    
            };

            gru_cell *cell;
            float Wrx[64];//4bit
            float Wrh[64];
            float Wzx[64];
            float Wzh[64];
            float Whx[64];
            float Whh[64];
            float Wo[64];
           
 
            int wrong_prediction_instruction = 0;
            int	correct_prediction_instruction = 0;
            float	precison = 0;
            bool predicte_result = 0;

            
            float sigmoid(float x){
                return (1 / (1 + exp(-x)));
            }

            bool GetPrediction(uint64_t pc){
                cell[127].Xin = pc;
                int *p = new int[64];
				int *pc_vector = p;
                pc_vector = slice(pc);
                float Rt = 0;
                float Zt = 0;
                float h_tmp[64];

                for(int i=0; i < 64; i++){
                    Rt = Rt + Wrh[i] * cell[126].hide_feature[i] + Wrx[i] * pc_vector[i];
                    Zt = Zt + Wzh[i] * cell[126].hide_feature[i] + Wzx[i] * pc_vector[i];
                }
                Rt = sigmoid((Rt-480)/960);
                Zt = sigmoid((Zt-480)/960);
                for(int i=0; i < 64; i++){
                    h_tmp[i] = Rt * Whh[i] * cell[126].hide_feature[i] + Whx[i] * pc_vector[i];
                    h_tmp[i] = tanh(h_tmp[i]);
                    cell[127].hide_feature[i] = (1 - Zt) * cell[126].hide_feature[i] + Zt * h_tmp[i];
                    if(cell[127].hide_feature[i] < 0)
                        cell[127].hide_feature[i] = 0;
                    if(cell[127].hide_feature[i] > 3)
                        cell[127].hide_feature[i] = 3;
                    cell[127].Yout = cell[127].Yout + Wo[i] * cell[127].hide_feature[i];
                }
                cell[127].Yout = sigmoid((cell[127].Yout - 1440)/2880);
                if(cell[127].Yout >= 0.5)
                    predicte_result = 1;
                else
                    predicte_result = 0;
				delete[] p;
                return predicte_result;
            }  
            
            void UpdatePredictor(bool ture_result){
                float error[64];
                int para;
                cell[127].iftaken = ture_result;
                if(ture_result != predicte_result){
                    if(ture_result == 0)
                        para = -1;
                    else
                        para = 1;
                    int j = 0;
                    for(int i = 0; i < 128; i = i + 2){
                        error[j] = (cell[i].Yout - cell[i].iftaken + cell[i+1].Yout - cell[i+1].iftaken)/2;
                        j++;
                    }
                    for (int i = 0; i < 64; i++){
                        Wrx[i] = Wrx[i] + error[i] * para;
                        Wrh[i] = Wrh[i] + error[i] * para;
                        Wzx[i] = Wzx[i] + error[i] * para;
                        Wzh[i] = Wzh[i] + error[i] * para;
                        Whx[i] = Whx[i] + error[i] * para;
                        Whh[i] = Whh[i] + error[i] * para;
                        Wo[i]  = Wo[i] + error[i] * para;

                        if(Wrx[i] > 15)
                            Wrx[i] = 15;
                        if(Wrx[i] < 0)
                            Wrx[i] = 0; 

                        if(Wrh[i] > 15)
                            Wrh[i] = 15;
                        if(Wrh[i] < 0)
                            Wrh[i] = 0; 

                        if(Wzx[i] > 15)
                            Wzx[i] = 15;
                        if(Wzx[i] < 0)
                            Wzx[i] = 0; 

                        if(Wzh[i] > 15)
                            Wzh[i] = 15;
                        if(Wzh[i] < 0)
                            Wzh[i] = 0; 

                        if(Whx[i] > 15)
                            Whx[i] = 15;
                        if(Whx[i] < 0)
                            Whx[i] = 0; 

                        if(Whh[i] > 15)
                            Whh[i] = 15;
                        if(Whh[i] < 0)
                            Whh[i] = 0;

                        if(Wo[i] > 15)
                            Wo[i] = 15;
                        if(Wo[i] < 0)
                            Wo[i] = 0;    
                    }
                    for(int i = 0; i < 127; i++){
                         cell[i] = cell[i+1];
                    }
                    cell[127].Yout = 0;
                    cell[127].iftaken = 0; 
                    for(int i = 0; i< 64; i++){
                        cell[127].hide_feature[i] = 0;
                    }
                }
            }

    };
