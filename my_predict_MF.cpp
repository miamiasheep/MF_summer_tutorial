#include<iostream>
#include<stdio.h>
#include<stdlib.h>

#define DATA_MAX 40000
#define USER_MAX 100000  // how many users
#define ITEM_MAX 600000   // how many items
#define HF 10

using namespace std;
FILE *user_bias;
FILE *item_bias;
FILE *p_result;
FILE *q_result;
FILE *average_rating;
FILE *predict;
FILE *test;

float user_pref[USER_MAX];
float item_pref[ITEM_MAX];
float p[USER_MAX][HF];
float q[ITEM_MAX][HF];

float average;
float temp;
float predict_value;

int temp_int;
int count;
int user,item;

void print_user_bias(){
    for(int i=0;i<USER_MAX;i++){
        cout<<user_pref[i]<<endl;
        system("pause");
    }
}
void print_item_bias(){
    for(int i=0;i<ITEM_MAX;i++){
        cout<<item_pref[i]<<endl;
        system("pause");
    }
}
void print_p(){
    for(int i=0;i<USER_MAX;i++){
        for(int j=0;j<HF;j++){
            cout<<p[i][j]<<" ";
        }
        cout<<endl;
        system("pause");
    }
}
void print_q(){
    for(int i=0;i<ITEM_MAX;i++){
        for(int j=0;j<HF;j++){
            cout<<q[i][j]<<" ";
        }
        cout<<endl;
        system("pause");
    }
}
void read_user_bias(){
    user_bias=fopen("user_bias.txt","r");
    count=0;
    while(fscanf(user_bias,"%f",&temp)!=EOF){
        user_pref[count]=temp;
        count++;
    }
    fclose(user_bias);
    //print_user_bias();
}
void read_item_bias(){
    item_bias=fopen("item_bias.txt","r");
    count=0;
    while(fscanf(item_bias,"%f",&temp)!=EOF){
        item_pref[count]=temp;
        count++;
    }
    fclose(item_bias);
    //print_item_bias();
}
void read_average(){
    average_rating=fopen("average.txt","r");
    fscanf(average_rating,"%f",&average);
    fclose(average_rating);
    //cout<<average<<endl;system("pause");
}
void read_p(){
    p_result=fopen("p.txt","r");
    count=0;
    while(fscanf(p_result,"%f",&temp)!=EOF){
        p[(count/HF)][(count%HF)]=temp;
        count++;
    }
    fclose(p_result);
    //print_p();
}
void read_q(){
    q_result=fopen("q.txt","r");
    count=0;
    while(fscanf(q_result,"%f",&temp)!=EOF){
        q[(count/HF)][(count%HF)]=temp;
        count++;
    }
    fclose(q_result);
}

float dot_pq(int user,int item){
    float sigma=0;
    for(int k=0;k<HF;k++){
        sigma=sigma+(p[user][k]*q[item][k]);
    }
    return sigma;
}
int main(){
    read_average();
    read_item_bias();
    //print_item_bias();
    read_user_bias();
    //print_user_bias();
    read_p();
    //print_p();
    read_q();
    //print_q();
    test=fopen("Test.txt","r");
    predict=fopen("predict.txt","w");
    count=0;
    while(fscanf(test,"%d",&temp_int)!=EOF){
        if((count%4)==0){
            user=temp_int;
        }
        if((count%4)==1){
            item=temp_int;
        }
        if((count%4)==2){
            predict_value=dot_pq(user,item)+user_pref[user]+item_pref[item]-2*average;
            if(predict_value>100)predict_value=100;
            if(predict_value<0)predict_value=0;
            fprintf(predict,"%f\n",predict_value);
            //cout<<predict_value<<endl;
            //system("pause");
        }
        count++;
        //system("pause");
    }
    fclose(predict);
    fclose(test);
}
