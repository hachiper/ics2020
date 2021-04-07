#include <stdint.h>
#include <stdio.h>
uint64_t multimod(uint64_t a, uint64_t b, uint64_t m) {
    uint64_t A[64],B[64],sum[128];
	int i=0,j=0;
    for (int y=0;y<128;y++){
        sum[y]=0;
    }
	for(;a>=1;i++){
		A[i]=a&1;
		a>>=1;
	}
	for(;b>=1;j++){
		B[j]=b&1;
		b>>=1;
	}
    int over=0;
	for (int p=0;p<128;p++){
		for (int k=0;k<i;k++){
			int q=p-k;
			if(q<0) break;
			else if (q>=0&&q<j){
				sum[p]+=A[k]&B[q];
			}
		}
		uint64_t o=sum[p];
		sum[p]=(sum[p]+over)&1;
		over=(over+o)>>1;
	}
  	uint64_t mod=0,sum1=0;
	for(int len2=i+j-1;len2>=0;len2--){
		if(mod<<1 < mod){
		mod<<=1;
		sum1=sum[len2]+mod-m;
		}
		else{
		mod<<=1;
		sum1=sum[len2]+mod;}
		while(sum1>=m){
			sum1=sum1-m;
		}
		mod=sum1;
}	
	return  mod;
}
