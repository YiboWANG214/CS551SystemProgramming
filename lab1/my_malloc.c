/*
THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
A TUTOR OR CODE WRITTEN BY OTHER STUDENTS – Yibo WANG
*/


#include "my_malloc.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
FreeListNode freelist=NULL;
MyErrorNo my_errno;

FreeListNode free_list_begin(void){
	return freelist;
}


// int binary_conversion( int value_t)
// {
//   int value = value_t;
//   int target_system = target_system_t;
//   int target_value [BASE_SIZE] = {0};
//   int target_value_i = 0;
//   while( value )
//   {
//    target_value[target_value_i] = value % target_system; 
//    value = value / target_system;
//    target_value_i++;
//   }
 
//     if( target_system == HEX )
//     {
//         for( ; target_value_i >= 0; target_value_i-- )
// 	    {
// 		  printf( "%x", target_value[target_value_i] );
// 	    }
//     }else{
// 	    for( ; target_value_i >= 0; target_value_i-- )
// 	    {
// 		  printf( "%d", target_value[target_value_i] );
// 	    }
// 	}
// }


void *find_prev(void* node){
	void* tmp;
	FreeListNode f;

	tmp = free_list_begin();
	if (node == tmp){
		return NULL;
	}
	f = tmp;
	while(f->flink != NULL){
		if ((void*) f->flink == node){
			return (void*)f;
		}
		f = f->flink;
	}
}

int align(int size){
	return ( ( ((size-1)>>3)<<3 ) + 8);
}

void *my_malloc(size_t size){
	FreeListNode curr, tmp, prev, freenode;
	void *result;
	void *begin;
	int s_size;
	long long int * start;
	long long int * desi;
	if ((int) size < 0){
		my_errno = 1;
		return NULL;
	}
	if ((int)size == 0){
		return NULL;
	}

	size = ((align(size) + CHUNKHEADERSIZE)>16?(align(size) + CHUNKHEADERSIZE):16);
	// printf("size is :%ld\n", size);
	curr = free_list_begin();

	if(curr == NULL){
		if(size <= 8192){
			s_size = 8192;
			begin = (void *) sbrk(s_size);
			if(begin == (void *) -1){
				my_errno = 1;
				return NULL;
			}
			if ((8192-size) <= size){
				size  = 8192;
			}
			if (freelist == NULL){
				freelist = (FreeListNode) (begin+size);
				freelist->size = s_size-size;
				freelist->flink = NULL;
			}
		}
		else{
			s_size = size;
			begin = (void *) sbrk(s_size);
			if(begin == (void *) -1){
				my_errno = 1;
				return NULL;
			}
		}
		result = (void *) (((char *)(begin))+8);

		start = (long long int *) (result-8);
		*start = ((size<<32)^255);

		return result;

	}
	else{

		// printf("%p\n", curr);
		// printf("%ld\n", curr->size);
		freenode = NULL;
		while (curr != NULL){
			if (curr->size >= size){
				freenode = curr;
				break;
			}
			curr = curr->flink;
		}
		if (freenode != NULL){
			if ((freenode->size-size)>=16){
				prev = find_prev(freenode);
				if (prev==NULL){
					freelist = (FreeListNode)((char *)freenode+size);
					freelist->size = freenode->size-size;
					freelist->flink = NULL;
				}
				else{
					prev->flink = (FreeListNode)((char *)freenode+  size);
					prev->flink->flink = freenode->flink;
					prev->flink->size = freenode->size-size;
				}
				freenode->size = size;
				freenode->flink = NULL;
			}
			else{
				prev = find_prev(freenode);
				if (prev==NULL){
					freelist = curr->flink;
				}
				else{
					prev->flink = freenode->flink;
					freenode->flink = NULL;
				}

			}
			// result = (void *) ((char *)(freenode)+8);
			result = (void *) ((char *)(freenode)+8);
			start = (long long int *) (result-8);
			*start = ((size<<32)^255);

			// printf("result is %p\n", (void *) result);
			// // printf("%lln\n", start);
			printf("%ld\n", size);
			// int k = 1;
			printf("%ld\n", (size<<32)^255);

			return result;
		}
		else{
			if(size <= 8192){
				s_size = 8192;
				begin = (void *) sbrk(s_size);
				if(begin == (void *) -1){
					my_errno = 1;
					return NULL;
				}
				if ((8192-size) <= size){
					size  = 8192;
				}
				if (freelist == NULL){
					freelist = (FreeListNode) (begin+size);
					freelist->size = s_size-size;
					freelist->flink = NULL;
				}
			}
			else{
				s_size = size;
				begin = (void *) sbrk(s_size);
				if(begin == (void *) -1){
					my_errno = 1;
					return NULL;
				}
			}
			result = (void *) ((char *)(begin)+8);

			start = (long long int *) (result-8);
			*start = ((size<<32)^255);

			return result;

		}
	}
	
}


void coalesce_free_list(void){
    FreeListNode curr, tmp;
    curr=freelist;
    size_t size;

    while ((curr->flink)!= NULL){
    	size = curr->size;
    	if (curr->flink && (((char *) curr + curr->size) == (char *)curr->flink)){
    		size += curr->flink->size;
    		tmp = curr->flink->flink;
    		curr->flink = tmp;
    		curr->size = size;
    	}
    }
}


void my_free(void *ptr){
	// printf("!!!!! %lld\n", *(long long int *) (ptr-4));
	
	if (*((int *)ptr-2) != 255){
		my_errno = 2;
		return;
	}

	FreeListNode new_node;
	new_node = (FreeListNode) ((char*) ptr-8);
	// printf(" here!!!! %d\n", *((int *)ptr-2));
	new_node->size = *((int *)ptr-1);
	new_node->flink = NULL;

	// printf("here !!!! %ld\n", new_node->size);
	// int i ;
	// memcpy(&i,(char*)((int *)ptr)-4,4);
	// printf("here!!!!! %d\n", i);
	// if ( (*(int *)ptr & 1) != 1){
	// 	my_errno = MYBADFREEPTR;
	// 	return;
	// }


	FreeListNode curr, prev;
	curr = (FreeListNode)free_list_begin();
	if(curr==NULL){
		freelist = new_node;
		return;
	}
	if(curr >= (FreeListNode) ptr){
		new_node->flink = freelist;
		freelist = new_node;
		return;
	}
	else{

		while(curr != NULL && curr < (FreeListNode) ptr){
			prev = curr;
			curr = curr->flink;
		}
		prev->flink = new_node;
		new_node->flink = curr;
		return;
	}
}

