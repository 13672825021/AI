#include <iostream>
#include <algorithm>
#include <cstring>
#include <string>
#include <cstdio>
#include <set>
#include <vector>
#include <ctime>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <fstream>
using namespace std;
bool savew=false;
int Length;//w��train�����ĳ��� 
int hidenum=70;
int iter_time;
bool Random_init=true;
double eta;
int traincnt;//������ 
int testcnt;
int valicnt;
double train[20000][100];//������5000��train���� 
double vali[10000][100];
double test[2500][100];
int trainlabel[20000];//ÿ�������ı�ǩ�������� 
int valilabel[10000];
double trainpredict[20000];
double testpredict[1000];
double valipredict[10000];
double MT[10000];//ѵ����MSE�½����� 
double MV[10000];//��֤��MSE�½�����  
double x[100];//ÿ��ȡһ��������x����洢�������������������ڵ����� 
double h[100];//���ز�ڵ�����
double y;//ֻ��һ�������ڵ�
double MSE;//ѵ����������� 
double MSE_of_vali;//��֤��������� 
double T_W_i2h[100][100];//����㵽���ز�
double T_W_h2o[100];//���ز㵽�����
double W_i2h[100][100];//����㵽���ز��Ȩ������ 
double W_h2o[100];//���ز㵽������Ȩ������
double delta_out;//���������ݶȣ���
double delta_hide[100];//���ز�����ݶȣ���j 
int test_date[1000];
void SetParameter(){
	eta=0.01;
	iter_time=1000;
	Random_init=true;
	savew=true;
}
int WhatDay(string str)
{
	string year="",month="",date="";
	int which=0;
	for(int j=0;j<str.size();j++){
		if(str[j]=='/') which++;
		if(which==0&&str[j]!='/')	year=year+str[j];
		else if(which==1&&str[j]!='/')	month+=str[j];
		else if(which==2&&str[j]!='/')	date+=str[j];
	}
	int yearnum,monthnum,datenum;
	stringstream sss;
	sss.clear();
	sss.str(year);
	sss>>yearnum;	
	sss.clear();
	sss.str(month);
	sss>>monthnum;
	sss.clear();
	sss.str(date);
	sss>>datenum;
	int day_code,s=0,a[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	day_code=(yearnum+(yearnum-1)/4-(yearnum-1)/100+(yearnum-1)/400)%7;
	for(int i=0;i<monthnum-1;i++)
		s=s+a[i];
	s=s+datenum;
	if(yearnum%4==0) s=s+1;
	int j=(s+day_code-1)%7;
	return j;
}
int WhatDate(string str)
{
	string year="",month="",date="";
	int which=0;
	for(int j=0;j<str.size();j++){
		if(str[j]=='/') which++;
		if(which==0&&str[j]!='/')	year=year+str[j];
		else if(which==1&&str[j]!='/')	month+=str[j];
		else if(which==2&&str[j]!='/')	date+=str[j];
	}
	int datenum;
	stringstream sss;
	sss.clear();
	sss.str(date);
	sss>>datenum;
	return datenum;
}
int WhatMonth(string str){
	string year="",month="",date="";
	int which=0;
	for(int j=0;j<str.size();j++){
		if(str[j]=='/') which++;
		if(which==0&&str[j]!='/')	year=year+str[j];
		else if(which==1&&str[j]!='/')	month+=str[j];
		else if(which==2&&str[j]!='/')	date+=str[j];
	}
	int yearnum,monthnum,datenum;
	stringstream sss;
	sss.clear();
	sss.str(month);
	sss>>monthnum;
	return monthnum;
}
int WhatYear(string str){
	string year="",month="",date="";
	int which=0;
	for(int j=0;j<str.size();j++){
		if(str[j]=='/') which++;
		if(which==0&&str[j]!='/')	year=year+str[j];
		else if(which==1&&str[j]!='/')	month+=str[j];
		else if(which==2&&str[j]!='/')	date+=str[j];
	}
	int yearnum,monthnum,datenum;
	stringstream sss;
	sss.clear();
	sss.str(year);
	sss>>yearnum;
	return yearnum;
}
void Readtrain()
{
	ifstream fin("train.csv");
	string line;
	vector<string> fields;
	traincnt=0;
	valicnt=0;
	int mo=8;
	int cnt=0;
	while(getline(fin,line))
	{
		cnt++;
		if(cnt==1) continue;
		fields.clear();
		istringstream sin(line);		
		string field;
		while(getline(sin,field,','))
		{
			fields.push_back(field);
		}	
		train[traincnt][0]=1;
		vali[valicnt][0]=1;
		int index=1;
		for(int i=0;i<fields.size();i++)
		{
			if(i==0) continue;
			double temnum;
			stringstream ss;
			ss.clear();
			ss.str(fields[i]);
			ss>>temnum;			
			if(cnt%mo==0)//vali 
			{
				if(i==1){
					int month=WhatMonth(fields[i]);
					int date=WhatDate(fields[i]);
					if(month==12&&(date>=22&&date<=30)) vali[valicnt][index++]=1;
					else vali[valicnt][index++]=0;
					
					int year=WhatYear(fields[i]);
					if(year==2011) vali[valicnt][index++]=0;
					else vali[valicnt][index++]=1;
					
					int day=WhatDay(fields[i]);
					for(int k=0;k<7;k++){
						if(k==day) vali[valicnt][index]=1;
						else vali[valicnt][index]=0;
						if(k!=6) index++;
					}
				}
				else if(i==2){			
					for(int k=0;k<24;k++){
						if(k==temnum) vali[valicnt][index]=1;
						else vali[valicnt][index]=0;
						if(k!=23) index++;
					}
				}
				else if(i==3){
					if(temnum==1){
						vali[valicnt][index]=1;vali[valicnt][index+1]=0;vali[valicnt][index+2]=0;
					}
					else if(temnum==2) {
						vali[valicnt][index]=0;vali[valicnt][index+1]=1;vali[valicnt][index+2]=0;
					}
					else{
						vali[valicnt][index]=0;vali[valicnt][index+1]=0;vali[valicnt][index+2]=1;
					} 
					index+=2;
				}
				else if(i==fields.size()-1) {
					if(temnum<2000){
						valilabel[valicnt]=temnum;
						valicnt++;
					}	
				}
				else vali[valicnt][index]=temnum;			
			}
			else//train 
			{				
				if(i==1){
					int month=WhatMonth(fields[i]);
					int date=WhatDate(fields[i]);
					if(month==12&&(date>=22&&date<=30)) train[traincnt][index++]=1;
					else train[traincnt][index++]=0;
					
					int year=WhatYear(fields[i]);
					if(year==2011) train[traincnt][index++]=0;
					else train[traincnt][index++]=1;
					
					int day=WhatDay(fields[i]);
					for(int k=0;k<7;k++){
						if(k==day) train[traincnt][index]=1;
						else train[traincnt][index]=0;
						if(k!=6) index++;
					}
				}
				else if(i==2){			
					for(int k=0;k<24;k++){
						if(k==temnum) train[traincnt][index]=1;
						else train[traincnt][index]=0;
						if(k!=23) index++;
					}
				}
				else if(i==3){
					if(temnum==1){
						train[traincnt][index]=1;train[traincnt][index+1]=0;train[traincnt][index+2]=0;
					}
					else if(temnum==2) {
						train[traincnt][index]=0;train[traincnt][index+1]=1;train[traincnt][index+2]=0;
					}
					else{
						train[traincnt][index]=0;train[traincnt][index+1]=0;train[traincnt][index+2]=1;
					} 
					index+=2;
				}
				else if(i==fields.size()-1) 
				{
					if(temnum<2000){
						trainlabel[traincnt]=temnum;
						traincnt++;
					}
				}
				else train[traincnt][index]=temnum;				
			}
			index++;
		}						
		Length=index-1;
	}
}

void Readtest()
{
	ifstream fin("test.csv");
	string line;
	vector<string> fields;
	testcnt=0;
	int cnt=0;
	while(getline(fin,line))
	{
		cnt++;
		if(cnt==1) continue;
		fields.clear();
		istringstream sin(line);		
		string field;
		while(getline(sin,field,',')){
			fields.push_back(field);
		}	
		test[testcnt][0]=1;
		int index=1;
		for(int i=0;i<fields.size()-1;i++)
		{
			if(i==0) continue;
			double temnum;
			stringstream ss;
			ss.clear();
			ss.str(fields[i]);
			ss>>temnum;	
			if(i==1){
				int month=WhatMonth(fields[i]);
				int date=WhatDate(fields[i]);
				if(month==12&&(date>=22&&date<=30)) test[testcnt][index++]=1;
				else test[testcnt][index++]=0;
				
				int year=WhatYear(fields[i]);
				if(year==2011) test[testcnt][index++]=0;
				else test[testcnt][index++]=1;
				
				
				int day=WhatDay(fields[i]);
				for(int k=0;k<7;k++){
					if(k==day) test[testcnt][index]=1;
					else test[testcnt][index]=0;
					if(k!=6) index++;
				}
				test_date[testcnt]=date;
			}
			else if(i==2){			
				for(int k=0;k<24;k++){
					if(k==temnum) test[testcnt][index]=1;
					else test[testcnt][index]=0;
					if(k!=23) index++;
				}
			}
			else if(i==3){
				if(temnum==1){
					test[testcnt][index]=1;test[testcnt][index+1]=0;test[testcnt][index+2]=0;
				}
				else if(temnum==2) {
					test[testcnt][index]=0;test[testcnt][index+1]=1;test[testcnt][index+2]=0;
				}
				else{
					test[testcnt][index]=0;test[testcnt][index+1]=0;test[testcnt][index+2]=1;
				} 
				index+=2;
			}
			else test[testcnt][index]=temnum;
			index++;
		}
		testcnt++;
	}
}
void readpreviousW() 
{
	ifstream fin("Winit.csv");
	for(int i=0;i<Length;i++)
		for(int j=0;j<hidenum;j++)
			fin>>W_i2h[i][j];
	for(int i=0;i<hidenum;i++) fin>>W_h2o[i];
}
void initialize_weight()
{
	if(Random_init==true){
		srand(time(0));	
		for(int i=0;i<Length;i++)//��ʼ������㵽���ز�֮��Ȩֵ����W_i2h
			for(int j=0;j<hidenum;j++)
				W_i2h[i][j]=rand()*1.0/RAND_MAX*2;	
		for(int i=0;i<hidenum;i++) W_h2o[i]=rand()*1.0/RAND_MAX*2;//��ʼ�����ز㵽�����֮��Ȩֵ����W_h2o
	}
	else readpreviousW();
}
void initialize_T_W()
{	
	for(int i=0;i<Length;i++)//��ʼ��T_W_i2h������Wij 
		for(int j=0;j<hidenum;j++)
			T_W_i2h[i][j]=0;
	for(int i=0;i<hidenum;i++) T_W_h2o[i]=0;//��ʼ��T_W_h2o������Wi
}
void forward_pass_i2h()//����㵽���ز� 
{
	h[0]=1;//Ϊ��ֵ��׼�� 
	for(int i=1;i<hidenum;i++)
	{
		double in=0;//���ȼ������뵽���ز�ڵ��ֵ 
		for(int j=0;j<Length;j++) in+=W_i2h[j][i]*x[j];
		h[i]=1/(1+exp(-1*in));//��sigmoid����Ϊ�������ȷ�����ز�ڵ���ֵ 
		//h[i]=( exp(in)-exp(-in) )/( exp(in)+exp(-in) );//tanh
	}
}
void forward_pass_h2o(int index)//���ز㵽����� 
{
	double in=0;//���ȼ������뵽�����ڵ��ֵ 
	for(int i=0;i<hidenum;i++) in+=W_h2o[i]*h[i];
	y=in;//�����Ժ���f(x)=xΪ�������ȷ�������ڵ���ֵ 
}
void backward_pass(int index)//���򴫵� 
{
	delta_out=trainlabel[index]-y;
	for(int i=0;i<hidenum;i++)
		delta_hide[i]=delta_out*W_h2o[i]*h[i]*(1-h[i]);	
		//delta_hide[i]=delta_out*W_h2o[i]*(1+h[i])*(1-h[i]);  //tanh 
	//�ۼ�Ȩֵ����W_h2o�ĸ���ֵ 
	for(int i=0;i<hidenum;i++) T_W_h2o[i]+=delta_out*h[i];	
	//�ۼ�Ȩֵ����W_i2h�ĸ���ֵ 
	for(int j=0;j<hidenum;j++) 
		for(int i=0;i<Length;i++)
			T_W_i2h[i][j]+=delta_hide[j]*x[i];
}
void Update_Weight()//����Ȩֵ���� 
{
	for(int j=0;j<hidenum;j++) //����W_i2h 
		for(int i=0;i<Length;i++)
			W_i2h[i][j]+=eta*T_W_i2h[i][j]/traincnt;
	for(int i=0;i<hidenum;i++) W_h2o[i]+=eta*T_W_h2o[i]/traincnt;	//����W_h2o 
}
void Use_train()
{
	MSE=0;
	for(int k=0;k<traincnt;k++)
	{
		h[0]=1;
		for(int i=1;i<hidenum;i++)
		{
			double sum=0;
			for(int j=0;j<Length;j++) sum+=W_i2h[j][i]*train[k][j];
			h[i]=1/(1+exp(-1*sum));
			//h[i]=( exp(sum)-exp(-sum) )/( exp(sum)+exp(-sum) );
		}
		double sum=0;
		for(int i=0;i<hidenum;i++) {
			sum+=W_h2o[i]*h[i];
		}
		if(sum<0) sum=10;
		trainpredict[k]=sum;
		MSE+=(sum-trainlabel[k])*(sum-trainlabel[k]);
	}
	MSE/=traincnt;
}
void Use_vali()
{
	for(int k=0;k<valicnt;k++)
	{
		h[0]=1;
		for(int i=1;i<hidenum;i++)
		{
			double sum=0;
			for(int j=0;j<Length;j++) sum+=W_i2h[j][i]*vali[k][j];
			h[i]=1/(1+exp(-1*sum));
		}
		double sum=0;
		for(int i=0;i<hidenum;i++) sum+=W_h2o[i]*h[i];
		if(sum<0) sum=10;
		valipredict[k]=sum;
		MSE_of_vali+=(sum-valilabel[k])*(sum-valilabel[k]);
	}
	MSE_of_vali/=valicnt;
}
void Use_test()
{
	int testpcnt=0;
	for(int k=0;k<testcnt;k++)
	{
		h[0]=1;
		for(int i=1;i<hidenum;i++)
		{
			double sum=0;
			for(int j=0;j<Length;j++) sum+=W_i2h[j][i]*test[k][j];
			h[i]=1/(1+exp(-1*sum));
		}
		double sum=0;
		for(int i=0;i<hidenum;i++) sum+=W_h2o[i]*h[i];
		if(sum<0) sum=5;
		if(test_date[k]>=22) sum=sum*0.6;
		testpredict[testpcnt++]=sum;
	}
}
void Output_testpredict()
{
	ofstream fout;
	fout.open("15352446_zhongzhanhui.csv");
	for(int i=0;i<testcnt;i++)
		fout<<(int)testpredict[i]<<endl;
	fout.close();
}
void OutputMSE()
{
	ofstream fout;
	fout.open("MSE.csv");
	for(int i=0;i<50;i++)	fout<<MT[i]<<',';
	fout<<endl;
	for(int i=0;i<50;i++)	fout<<MV[i]<<',';
	fout.close();
}
void Output_compare()
{
	ofstream fout;
	fout.open("traincompare.csv");
	int start=0,end=traincnt;
	start=1000;end=1700;
	for(int i=start;i<end;i++)		
		fout<<trainlabel[i]<<',';
	fout<<endl;	
	for(int i=start;i<end;i++)
		fout<<trainpredict[i]<<',';
	fout<<endl;	
	fout.close();
	fout.open("valicompare.csv");
	start=0;end=valicnt;
	//start=1000;end=1300;
	for(int i=start;i<end;i++)		
		fout<<valilabel[i]<<',';
	fout<<endl;	
	for(int i=start;i<end;i++)
		fout<<valipredict[i]<<',';
	fout<<endl;			
	fout.close();
}
void nomalization(){
	for(int i=0;i<Length;i++){
		double maxnum=0,minnum=10000;
		for(int j=0;j<traincnt;j++){
			if(train[j][i]>maxnum) maxnum=train[j][i];
			if(train[j][i]<minnum) minnum=train[j][i];
		}
		for(int j=0;j<traincnt;j++){
			if(maxnum!=minnum) train[j][i]=(train[j][i]-minnum)/(maxnum-minnum);
		}
	}
	for(int i=0;i<Length;i++){
		double maxnum=0,minnum=10000;
		for(int j=0;j<valicnt;j++){
			if(vali[j][i]>maxnum) maxnum=vali[j][i];
			if(vali[j][i]<minnum) minnum=vali[j][i];
		}
		for(int j=0;j<valicnt;j++){
			if(maxnum!=minnum) vali[j][i]=(vali[j][i]-minnum)/(maxnum-minnum);
		}
	}	
	for(int i=0;i<Length;i++){
		double maxnum=0,minnum=10000;
		for(int j=0;j<testcnt;j++){
			if(test[j][i]>maxnum) maxnum=test[j][i];
			if(test[j][i]<minnum) minnum=test[j][i];
		}
		for(int j=0;j<testcnt;j++){
			if(maxnum!=minnum)	test[j][i]=(test[j][i]-minnum)/(maxnum-minnum);
		}
	}
}
void saveW()
{
	ofstream fout;
	fout.open("Winit.csv");
	for(int i=0;i<Length;i++)
		for(int j=0;j<hidenum;j++)
			fout<<W_i2h[i][j]<<endl;
	for(int i=0;i<hidenum;i++) fout<<W_h2o[i]<<endl;
	fout.close();
}

int main()
{
	SetParameter();//���ø��ֲ�����ѧϰ����������������ѡ��W�����ʼ�����Ƕ�ȡ�ϴα���������W 
	Readtrain();//��ȡѵ������������ѵ����ÿ8������ȡǰ7����Ϊѵ��������8����Ϊ��֤��
	Readtest();//��ȡ���Լ� 
	nomalization();//����ֵ��һ�� 
	cout<<"Length="<<Length<<"traincnt="<<traincnt<<" valicnt="<<valicnt<<" testcnt="<<testcnt<<endl;
	initialize_weight();//��ʼ��Ȩֵ���� Wij�� Wi
	for(int k=0;k<iter_time;k++)//����iter_time�� 
	{
		MSE=0;//ѵ������MSE 
		MSE_of_vali=0;//��֤����MSE 
		initialize_T_W();//��ʼ����Wij�͡�WiΪȫ������ 
		for(int i=0;i<traincnt;i++)//���ݶ��½�����������ѵ���� 
		{	
			for(int j=0;j<Length;j++) x[j]=train[i][j];//ȡ����ǰ������������������ֵ�Ž�x���� 
			forward_pass_i2h();	//���򴫵ݣ�������㵽���ز� 
			forward_pass_h2o(i);//���򴫵ݣ������ز㵽����� 
			backward_pass(i);//���򴫵ݣ��ۼơ�Wij�͡�Wi
		}
		Update_Weight();//ÿ������һ��ѵ��������һ��Ȩֵ���� 
		if(k%10==0){//ÿ����ʮ�μ��㲢���ѵ����MSE�鿴ѧϰЧ�� 
			Use_train();//�õ�ǰ��Ȩֵ����Ԥ��ѵ�������������MSE 
			Use_vali();//�õ�ǰ��Ȩֵ����Ԥ����֤�����������MSE 
			MT[k/10]=MSE;	
			MV[k/10]=MSE_of_vali;
			cout<<"ѵ����MSE:"<<MSE<<endl;
		}
	}
	OutputMSE();//��MSE����������ļ� 
	Output_compare();//��ѵ��������Ԥ��cnt����ʵcnt�����ͬһ���ļ��жԱ� 
	Use_test();//ʹ��Ȩֵ����Ԥ�� ���Լ� 
	Output_testpredict();//�����Լ�Ԥ����������ļ�����Ҫ�ύ��csv�ļ���	
	if(savew==true) saveW();//��Ȩֵ������������ 
} 










