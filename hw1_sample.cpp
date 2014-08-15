#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define HF 10       // hidden factor(latent class)
#define LR 0.000005    // learning rate
#define LAMDA 0.00000000 //regularization term
#define DATA_MAX 10000000   // how many rating data(In this case, there are about 2.3 millions rating data)
#define DATA_MAX_VAL 100000
#define USER_MAX 100000  // how many users
#define ITEM_MAX 60000   // how many items
#define TIME_MAX 1           // how many time frames (I don't want to use temporal bias)
#define TIME_DIV 800000000
#define TIME_NOR 0
using namespace std;


struct Data{
    int userID;
    int itemID;
    float rating;
    int time;
    void print(){
        cout<<" user id: "<<userID;
        cout<<" item id: "<<itemID;
        cout<<" rating: "<<rating;
        cout<<" time: "<<time;
        cout<<endl;
    }
};

int iteration;
int temp,count,count_val;
float ave_rating;
float user_pref[USER_MAX];
float user_pref_count[USER_MAX];
float item_pref[ITEM_MAX];
float item_pref_count[ITEM_MAX];
float time_pref[TIME_MAX];
float time_pref_count[TIME_MAX];


FILE *input_train;
FILE *input_val;
FILE *accuracy;
FILE *user_bias;
FILE *item_bias;
FILE *p_result;
FILE *q_result;
FILE *average_rating;

Data *train;
Data *val;
float p[USER_MAX][HF];
float q[ITEM_MAX][HF];

void print_train(){
    for(int i=0;i<count/4;i++){
        train[i].print();
        system("pause");
    }
}
void print_val(){
    for(int i=0;i<count_val/4;i++){
        val[i].print();
        system("pause");
    }
}
float square(float x){
    return x*x;
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
float dot_pq(int user,int item){
    float sigma=0;
    for(int k=0;k<HF;k++){
        sigma=sigma+(p[user][k]*q[item][k]);
    }
    return sigma;
}
float RMSE_test(){
     float predict;
    int n=(count_val/4);
    float sigma=0;
    int user,item,time;
    float rating;
    for(int i=0;i<n;i++){
        user=val[i].userID;
        item=val[i].itemID;
        time=train[i].time;
        rating=val[i].rating;
        predict=(user_pref[user]+item_pref[item])-ave_rating;
        if(predict>100)predict=100;
        if(predict<0)predict=0;
        sigma=sigma+(predict-rating)*(predict-rating);
    }
    return (float)sqrt(sigma/n);
}
float RMSE(){
    int n=(count/4);
    float predict;
    float sigma=0;
    int user,item;
    float rating;
    int time;
    for(int i=0;i<n;i++){
        user=train[i].userID;
        item=train[i].itemID;
        time=train[i].time;
        rating=train[i].rating;
        predict=dot_pq(user,item)+user_pref[user]+item_pref[item]/*+time_pref[(time/TIME_DIV)-TIME_NOR]*/-2*ave_rating;
        if(predict>100)predict=100;
        if(predict<0)predict=0;
        sigma=sigma+(predict-rating)*(predict-rating);
    }
    return (float)sqrt(sigma/n);
}
float RMSE_val(){
    float predict;
    int n=(count_val/4);
    float sigma=0;
    int user,item,time;
    float rating;
    for(int i=0;i<n;i++){
        user=val[i].userID;
        item=val[i].itemID;
        time=train[i].time;
        rating=val[i].rating;
        predict=dot_pq(user,item)+user_pref[user]+item_pref[item]/*+time_pref[(time/TIME_DIV)-TIME_NOR]*/-2*ave_rating;
        if(predict>100)predict=100;
        if(predict<0)predict=0;
        sigma=sigma+(predict-rating)*(predict-rating);
    }
    return (float)sqrt(sigma/n);
}

void random_ini(){
    for(int i=0;i<USER_MAX;i++){
        for(int j=0;j<HF;j++){
            p[i][j]=sqrt(ave_rating/HF)+0.001*((float)((rand()*i*243245+j*3432134*rand()+3)%RAND_MAX)/RAND_MAX)-0.0005;
            //p[i][j]=0;
            //cout<<sqrt((float)ave_rating/(float)HF);
            //system("pause");
        }
    }
    for(int i=0;i<ITEM_MAX;i++){
        for(int j=0;j<HF;j++){
            q[i][j]=sqrt(ave_rating/HF)+0.001*((float)((rand()*i*321432+j*rand()*483729+2)%RAND_MAX)/RAND_MAX)-0.0005;
            //q[i][j]=0;
        }
    }

}
int main(){
    //srand(time(0));
    input_train = fopen("Train.txt","r");
    train = new Data[DATA_MAX];
    count=0;
    while(fscanf(input_train,"%d",&temp)!=EOF){
        if((count%4)==0){
            train[count/4].userID = temp;
        }
        else if((count%4)==1){
            train[count/4].itemID = temp;
        }
        else if((count%4)==2){
            train[count/4].rating = temp;
        }
        else if ((count%4==3)){
            train[count/4].time = temp;
        }
        count++;
        //cout<<count<<endl;
    }
    fclose(input_train);
    ///////////////////////////////////
    input_val = fopen("Valid.txt","r");
    val = new Data[DATA_MAX_VAL];
    count_val=0;
    while(fscanf(input_val,"%d",&temp)!=EOF){
        if((count_val%4)==0){
            val[count_val/4].userID = temp;
        }
        else if((count_val%4)==1){
            val[count_val/4].itemID = temp;
        }
        else if((count_val%4)==2){
            val[count_val/4].rating = temp;
        }
        else if ((count_val%4==3)){
            val[count_val/4].time = temp;
        }
        count_val++;
        //cout<<count<<endl;
    }
    fclose(input_val);

    ///////////////////////////////////////////////

    int user,item,time;
    float rating;
    float last_RMSE;
    /////////////////calculate preference
    for(int i=0;i<USER_MAX;i++){
        user_pref[i]=0;
        user_pref_count[i]=0;
    }
    for(int i=0;i<ITEM_MAX;i++){
        item_pref[i]=0;
        item_pref_count[i]=0;
    }
    for(int i=0;i<TIME_MAX;i++){
        time_pref[i]=0;
        time_pref_count[i]=0;
    }
    ave_rating=0;
    for(int i=0;i<count/4;i++){
        user=train[i].userID;
        item=train[i].itemID;
        time=train[i].time;
        rating=train[i].rating;

        ave_rating+=rating;
        user_pref[user]+=rating;
        user_pref_count[user]++;
        item_pref[item]+=rating;
        item_pref_count[item]++;
        time_pref[(time/TIME_DIV)-TIME_NOR]+=rating;
        time_pref_count[(time/TIME_DIV)-TIME_NOR]++;
    }
    ave_rating=ave_rating/(count/4);
    random_ini();

    for(int i=0;i<USER_MAX;i++){
        if(user_pref_count[i]!=0)user_pref[i]=((user_pref[i]+ave_rating*0.1)/(user_pref_count[i]+0.3));
        else user_pref[i]=ave_rating;
    }
    for(int i=0;i<ITEM_MAX;i++){
        if(item_pref_count[i]!=0)item_pref[i]=((item_pref[i]+ave_rating*5)/(item_pref_count[i]+5));
        else item_pref[i]=ave_rating;
    }
    for(int i=0;i<TIME_MAX;i++){
        if(time_pref_count!=0)time_pref[i]=(time_pref[i]/time_pref_count[i]);
        else time_pref[i]=ave_rating;
    }

    ////////////////calculate preference

    iteration=0;
    cout<<"training start..."<<endl;
    while(true){
        for(int i=0;i<(count/4);i++){
            user=train[i].userID;
            item=train[i].itemID;
            time=train[i].time;
            rating=train[i].rating;
            for(int k=0;k<HF;k++){
                p[user][k]=p[user][k]-LR*(-(rating-user_pref[user]-item_pref[item]+(2*ave_rating)-dot_pq(user,item))*q[item][k])+LAMDA*p[user][k];
                q[item][k]=q[item][k]-LR*(-(rating-user_pref[user]-item_pref[item]+(2*ave_rating)-dot_pq(user,item))*p[user][k])+LAMDA*q[item][k];
            }
        }
        if((RMSE_val()<28)&&(RMSE_val()>last_RMSE)){
            cout<<"trainning error:"<<RMSE()<<endl;
            cout<<"CV error "<<RMSE_val()<<endl;
            iteration++;
            break;
        }
        last_RMSE=RMSE_val();
        cout<<"trainning error:"<<RMSE()<<endl;
        cout<<"CV error "<<RMSE_val()<<endl;
        //break;
        iteration++;
        cout<<iteration<<endl;
    }
    accuracy=fopen("CVerror.txt","w");
    fprintf(accuracy,"CVerror: %f\n",RMSE_val());
    fprintf(accuracy,"training error: %f\n",RMSE());
    fprintf(accuracy,"difference: %f\n",RMSE_val()-RMSE());
    fclose(accuracy);
    user_bias=fopen("user_bias.txt","w");
    for(int i=0;i<USER_MAX;i++){
        fprintf(user_bias,"%f\n",user_pref[i]);
    }
    fclose(user_bias);
    item_bias=fopen("item_bias.txt","w");
    for(int i=0;i<ITEM_MAX;i++){
        fprintf(item_bias,"%f\n",item_pref[i]);
    }
    fclose(item_bias);
    p_result=fopen("p.txt","w");
    for(int i=0;i<USER_MAX;i++){
        for(int j=0;j<HF;j++){
            fprintf(p_result,"%f ",p[i][j]);
        }
        fprintf(p_result,"\n");
    }
    fclose(p_result);
    q_result=fopen("q.txt","w");
    for(int i=0;i<ITEM_MAX;i++){
        for(int j=0;j<HF;j++){
            fprintf(q_result,"%f ",q[i][j]);
        }
        fprintf(q_result,"\n");
    }
    fclose(q_result);
    average_rating=fopen("average.txt","w");
    fprintf(average_rating,"%f",ave_rating);
    fclose(average_rating);
    //////////////////////////////////////////////////////////////////////////////////////////retrain

    cout<<"retrain start..."<<endl;
    //random_ini();
    //iteration=319;
    for(int i=0;i<USER_MAX;i++){
        user_pref[i]=0;
        user_pref_count[i]=0;
    }
    for(int i=0;i<ITEM_MAX;i++){
        item_pref[i]=0;
        item_pref_count[i]=0;
    }
    for(int i=0;i<TIME_MAX;i++){
        time_pref[i]=0;
        time_pref_count[i]=0;
    }
    ave_rating=0;
    for(int i=0;i<count/4;i++){
        user=train[i].userID;
        item=train[i].itemID;
        time=train[i].time;
        rating=train[i].rating;

        ave_rating+=rating;
        user_pref[user]+=rating;
        user_pref_count[user]++;
        item_pref[item]+=rating;
        item_pref_count[item]++;
        time_pref[(time/TIME_DIV)-TIME_NOR]+=rating;
        time_pref_count[(time/TIME_DIV)-TIME_NOR]++;
    }
    for(int i=0;i<count_val/4;i++){
        user=val[i].userID;
        item=val[i].itemID;
        time=val[i].time;
        rating=val[i].rating;

        ave_rating+=rating;
        user_pref[user]+=rating;
        user_pref_count[user]++;
        item_pref[item]+=rating;
        item_pref_count[item]++;
        time_pref[(time/TIME_DIV)-TIME_NOR]+=rating;
        time_pref_count[(time/TIME_DIV)-TIME_NOR]++;
    }
    ave_rating=ave_rating/((count+count_val)/4);
    for(int i=0;i<USER_MAX;i++){
        if(user_pref_count[i]!=0)user_pref[i]=((user_pref[i]+ave_rating*0.1)/(user_pref_count[i]+0.1));
        else user_pref[i]=ave_rating;
    }
    for(int i=0;i<ITEM_MAX;i++){
        if(item_pref_count[i]!=0)item_pref[i]=((item_pref[i]+ave_rating*5)/(item_pref_count[i]+5));
        else item_pref[i]=ave_rating;
    }
    for(int i=0;i<TIME_MAX;i++){
        if(time_pref_count!=0)time_pref[i]=(time_pref[i]/time_pref_count[i]);
        else time_pref[i]=ave_rating;
    }
    random_ini();
    ////////////////calculate preference
    //iteration=0;
    for(int iter=0;iter<iteration;iter++){
        for(int i=0;i<(count/4);i++){
            user=train[i].userID;
            item=train[i].itemID;
            time=train[i].time;
            rating=train[i].rating;
            for(int k=0;k<HF;k++){
                p[user][k]=p[user][k]-LR*(-(rating-user_pref[user]-item_pref[item]+(2*ave_rating)-dot_pq(user,item))*q[item][k])+LAMDA*p[user][k];
                q[item][k]=q[item][k]-LR*(-(rating-user_pref[user]-item_pref[item]+(2*ave_rating)-dot_pq(user,item))*p[user][k])+LAMDA*q[item][k];
            }
        }
        for(int i=0;i<(count_val/4);i++){
            user=val[i].userID;
            item=val[i].itemID;
            time=val[i].time;
            rating=val[i].rating;
            for(int k=0;k<HF;k++){
                p[user][k]=p[user][k]-LR*(-(rating-user_pref[user]-item_pref[item]+(2*ave_rating)-dot_pq(user,item))*q[item][k])+LAMDA*p[user][k];
                q[item][k]=q[item][k]-LR*(-(rating-user_pref[user]-item_pref[item]+(2*ave_rating)-dot_pq(user,item))*p[user][k])+LAMDA*q[item][k];
            }
        }
        if((RMSE_val()<32)&&(RMSE_val()>last_RMSE)){
            cout<<"trainning error:"<<RMSE()<<endl;
            cout<<"CV error "<<RMSE_val()<<endl;
            iteration++;
            //break;
        }
        last_RMSE=RMSE_val();
        cout<<"trainning error:"<<RMSE()<<endl;
        cout<<"CV error "<<RMSE_val()<<endl;
        cout<<iter<<endl;
    }

    return 0;
}
