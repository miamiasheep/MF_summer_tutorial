MF_summer_tutorial
==================

如果是修summer tutorial的學生，

可以從hw1_sample.cpp那份檔案開始改。

或是自己打造一份。

如果單純使用hw1_sample.cpp，你可以得到

train RMSE: 21.1963
valid RMSE: 26.8933  
(可能會依據random seed有一小點微幅變動)

我發現我之前的更改並不會讓我的RMSE高到28以上，
所以這次作業就變成什麼都不用做，就可以達成基本目標囉XD。

當然，你可以打造一份更好的!!!
這樣我也會給你的作業更多的回饋唷!!!

===================

matrix factorization code for summer tutorial


my_train_MF

input:

     1. Train.txt 
	 2. Valid.txt

output:

     1. p.txt
	 2. q.txt  
	 3. average.txt
	 4. user_bias.txt
	 5. item_bias.txt
	 6. CVerror.txt

my_predict_MF

input:

	1. p.txt  
	2. q.txt 
	3. average.txt
	4. user_bias.txt
	5. item_bias.txt
	6. Test.txt
	
output: 
	
	1. predict.txt
