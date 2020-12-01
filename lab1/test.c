#include<string.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include "my_malloc.h"
int main(){

    // int *p = (int*)my_malloc(100*sizeof(int));
    // printf("%d\n",*(p-1));

    printf("Test 1 in here: just allocate the 40 byte which should get 48 byte chunk because header\n");
    int* p = (int*)my_malloc(40);
    printf("%d\n",*(p-1));
    int i ;
    // i = (int) (*(p-1));
    // printf("____\n");
    // printf("*(p-1) is %p\n", (long long int*)(p-2));

    memcpy(&i,(char*)p - 8,4);
    // printf("i is %d\n", i);
    assert(i == 48);
    p[0] = 1;
    p[1] = 2;
    p[2] = 3;
    assert(p[0] == 1);
    assert(p[1] == 2);
    assert(p[2] == 3);
    FreeListNode n = free_list_begin();
    while(n!=NULL){
        printf("free1 %p\n",n);
        printf("free1 %zu\n",n->size);
        n = n -> flink;
    }
    my_free(p);
    // printf("After free1\n");
    // printf("You need to check every freelistnode's address is chunked in right size\n");
    // printf("In here you have have 2 nodes and the size is 48 and 8144\n");
    n = free_list_begin();
    while(n!=NULL){
        printf("free1 %p\n",n);
        printf("free1 %zu\n",n->size);
        n = n -> flink;
    }




    printf("Test 2 in here: just allocate the 200 byte which should get 208 byte chunk because header\n");
    int* q = (int*)my_malloc(200);
    // memcpy(&i,(char*)p -8,4);
    printf("%d\n",*(q-1));
    i = (int) (*(q-1));
    assert(i==208);
    q[0] = 1;
    q[1] = 2;
    q[2] = 3;
    q[3] = 4;
    assert(q[0] == 1);
    assert(q[1] == 2);
    assert(q[2] == 3);
    assert(q[3] == 4);
    n = free_list_begin();
    while(n!=NULL){
        printf("free2 %p\n",n);
        printf("free2 %zu\n",n->size);
        n = n -> flink;
    }
    my_free(q);
    // printf("After free2\n");
    // printf("You need to check every freelistnode's address is chunked in right size\n");
    // printf("In here you have have 2 node and the size is 48 and 208\n");
    n = free_list_begin();
    while(n!=NULL){
        printf("free2 %p\n",n);
        printf("free2 %zu\n",n->size);
        n = n -> flink;
    }
    
    printf("Test 3 in here: just allocate the 400 byte which should get 408 byte chunk because header\n");
    // printf("In here you have have 2 node and the size is 24 and 8168\n");
    int* b = (int*)my_malloc(400);
    // memcpy(&i,(char*)b -8,4);
    printf("%d\n",*(b-1));
    i = (int) (*(b-1));
    assert(i == 408);
    n = free_list_begin();
    while(n!=NULL){
        printf("free3 %p\n",n);
        printf("free3 %zu\n",n->size);
        n = n -> flink;
    }
    my_free(b);
    // printf("After free3\n");
    n = free_list_begin();
    // printf("You need to check every freelistnode's address is chunked in right size\n");
    // printf("In here you have have 3 node and the size is 48， 208 and 408\n");
    while(n!=NULL){
        printf("free3 %p\n",n);
        printf("free3 %zu\n",n->size);
        n = n -> flink;
    }

    printf("Test 4 in here: just allocate the 20 byte which should get 32 byte chunk because header\n");
    int* w1 = (int*)my_malloc(40);
    // memcpy(&i,(int*)w-1,4);
    i = (int) (*(w1-1));
    assert(i == 48);
    int* w2 = (int*)my_malloc(200);
    // memcpy(&i,(int*)w-1,4);
    i = (int) (*(w2-1));
    assert(i == 208);
    int* w3 = (int*)my_malloc(400);
    // memcpy(&i,(int*)w-1,4);
    i = (int) (*(w3-1));
    assert(i == 408);


    my_free(w3);
    // printf("After free4\n");
    // printf("You need to check every freelistnode's address is chunked in right size\n");
    // printf("In here you have have 1 node and the size is 48\n");
    n = free_list_begin();
    while(n!=NULL){
        printf("free4 %p\n",n);
        printf("free4 %zu\n",n->size);
        n = n -> flink;
    }
    my_free(w2);
    // printf("After free4\n");
    // printf("You need to check every freelistnode's address is chunked in right size\n");
    // printf("In here you have have 1 node and the size is 208\n");
    n = free_list_begin();
    while(n!=NULL){
        printf("free4 %p\n",n);
        printf("free4 %zu\n",n->size);
        n = n -> flink;
    }   
    my_free(w1);
    // printf("After free4\n");
    // printf("You need to check every freelistnode's address is chunked in right size\n");
    // printf("In here you have have 1 node and the size is 408\n");
    n = free_list_begin();
    while(n!=NULL){
        printf("free4 %p\n",n);
        printf("free4 %zu\n",n->size);
        n = n -> flink;
    } 

    printf("Test 5 in here: just allocate the 565 byte which should get 576 byte chunk because header\n");
    int* b2= (int*)my_malloc(565);
    // memcpy(&i,(char*)a -8,4);
    i = (int) (*(b2-1));
    assert(i == 576);
    n = free_list_begin();
    while(n!=NULL){
        printf("free5 %p\n",n);
        printf("free5 %zu\n",n->size);
        n = n -> flink;
    } 
    my_free(b2);
    printf("After free5\n");
    n = free_list_begin();
    // printf("In here you have 2 nodes and the size is 32 and 8160\n");
    while(n!=NULL){
        printf("free5 %p\n",n);
        printf("free5 %zu\n",n->size);
        n = n -> flink;
    }

    int* b3= (int*)my_malloc(999);
    // memcpy(&i,(char*)a -8,4);
    i = (int) (*(b3-1));
    printf("%d\n",*(b3-1));
    assert(i == 1008);
    my_free(b3);
    printf("After free6\n");
    n = free_list_begin();
    // printf("In here you have 2 nodes and the size is 32 and 8160\n");
    while(n!=NULL){
        printf("free6 %p\n",n);
        printf("free6 %zu\n",n->size);
        n = n -> flink;
    }

    printf("_________________\n");
    printf("Test 5 in here: just allocate the 333 byte which should get 576 byte chunk because header\n");
    int* b4= (int*)my_malloc(333);
    // memcpy(&i,(char*)a -8,4);
    i = (int) (*(b4-1));
    assert(i == 344);
    my_free(b4);
    printf("After free6\n");
    n = free_list_begin();
    printf("In here you have 2 nodes and the size is 32 and 8160\n");
    while(n!=NULL){
        printf("free6 %p\n",n);
        printf("free6 %zu\n",n->size);
        n = n -> flink;
    }

    printf("Test 6 in here: just allocate the 999 byte which should get 1008 byte chunk because header\n");
    int* b5= (int*)my_malloc(2121);
    // memcpy(&i,(char*)a -8,4);
    i = (int) (*(b5-1));
    assert(i == 2136);
    my_free(b5);
    printf("After free6\n");
    n = free_list_begin();
    printf("In here you have 2 nodes and the size is 32 and 8160\n");
    while(n!=NULL){
        printf("free6 %p\n",n);
        printf("free6 %zu\n",n->size);
        n = n -> flink;
    }

    // printf("Test 6 in here: just allocate the 20 byte which should get 32 byte chunk because header\n");
    // int* a= (int*)my_malloc(sizeof(int)*5);
    // // memcpy(&i,(char*)a -8,4);
    // i = (int) (*(a-1));
    // assert(i == 32);
    // my_free(a);
    // printf("After free6\n");
    // n = free_list_begin();
    // printf("In here you have 2 nodes and the size is 32 and 8160\n");
    // while(n!=NULL){
    //     printf("free6 %p\n",n);
    //     printf("free6 %zu\n",n->size);
    //     n = n -> flink;
    // }

    printf("Test 7 in here: just allocate the 8192 byte which should get 8200 byte chunk because header\n");
    int* s = (int*)my_malloc(12000);
    // memcpy(&i,(char*)s -8,4);
    i = (int) (*(s-1));
    printf("%d\n", i);
    assert(i == 12008);
    my_free(s);
    printf("After free7\n");
    n = free_list_begin();
    printf("In here you have 3 nodes and the size is 32, 8160 and 8200\n");
    while(n!=NULL){
        printf("free7 %p\n",n);
        printf("free7 %zu\n",n->size);
        n = n -> flink;
    }

    printf("Test5 we will coalesce the free list node in here\n");
    coalesce_free_list();
    printf("After coalesce, just one node and size is 8192\n");
    n = free_list_begin();
    while(n!=NULL){
        printf("Coalesce %p\n",n);
        printf("Coalesce %zu\n",n->size);
        n = n -> flink;
    }



    printf("Test 8 in here: just allocate the 63 byte which should get 72 byte chunk because header\n");
    int *s1 = (int*)my_malloc(0);
    // memcpy(&i,(int*)s -8,4);
    // i = (int) (*(s1-1));
    // assert(i == 16);
    // // my_free(s);
    // printf("After free8\n");
    n = free_list_begin();
    printf("In here you have 4 nodes and the size is 32, 72, 8088 and 8200\n");
    while(n!=NULL){
        printf("free8 %p\n",n);
        printf("free8 %zu\n",n->size);
        n = n -> flink;
    }
    
    printf("Test 9 in here: just allocate the -1 \n");
    int *kk = (int*)my_malloc(-1);
    // memcpy(&i,(int*)s -8,4);
    extern MyErrorNo my_errno;
    assert(my_errno == MYENOMEM);


     // error case
    int* u = (int*)sbrk(103);
    my_free(u);
    // printf("%d\n", my_errno);
    assert(my_errno == MYBADFREEPTR);

    // printf("Test 9 in here: allocate the 20, 63, 8080, and 8192 bytes and because  header you should get 32, 72, 8088 and 8200 bytes \n");
    // int* s1 = (int*)my_malloc(sizeof(char)*20);
    // int* s2 = (int*)my_malloc(sizeof(char)*63);
    // int* s3 = (int*)my_malloc(sizeof(char)*8080);
    // int* s4 = (int*)my_malloc(sizeof(char)*8192);
    // printf("%d\n", (int) (*(s1-1)));
    // printf("%p\n", s1);
    // printf("%d\n", (int) (*(s2-1)));
    // printf("%p\n", s2);
    // printf("%d\n", (int) (*(s3-1)));
    // printf("%p\n", s3);
    // printf("%d\n", (int) (*(s4-1)));
    // printf("%p\n", s4);
    // assert(s2 - s1 == 32);
    // assert(s3 - s2 == 72);
    // assert(s4 - s3 == 8088 + 103);
    // s3[0] = 'a';
    // s3[1] = 'b';
    // s3[2] = 'c';
    // s3[3] = '\0';
    // assert(s3[0]=='a');
    // assert(s3[1] == 'b');
    // assert(s3[2] == 'c');
    // n = free_list_begin();
    // printf("In here you have 0 node\n");
    // while(n!=NULL){
    //     printf("free9 %p\n",n);
    //     printf("free9 %zu\n",n->size);
    //     n = n -> flink;
    // }
    
    
    // printf("Test 10 in here: allocate 55 bytes and get 72 bytes because of potential wastage\n");
    // my_free(s2);
    // my_free(s3);
    // n = free_list_begin();
    // printf("In here you have 2 nodes and size is 72 and 8088\n");
    // while(n!=NULL){
    //     printf("free10 %p\n",n);
    //     printf("free10 %zu\n",n->size);
    //     n = n -> flink;
    // }
    // int* s5 = (int*)my_malloc(sizeof(char)*55);
    // assert(s2 == s5);
    // printf("%p %p\n",s2,s5);
    // return 0;
}