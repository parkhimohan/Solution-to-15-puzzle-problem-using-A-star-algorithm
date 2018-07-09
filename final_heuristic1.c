#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
#define N 4

typedef struct board{
	int b[16], board_num, misplaced_tiles, f_n;
} BOARD;	

typedef struct node{
	BOARD B;
	struct node *next;
} NODE;

BOARD goal_state;
int puzzle[16], best_possible_solution[16], board_count=1, time_taken=1, space_taken=1, final_f_n;
// creating open and close lists
NODE *open_list, *close_list;
void print(int *arr, int board_num, int misplaced_tiles){
	int i;
	printf("Board number: %d, Misplaced tiles: %d", board_num, misplaced_tiles);
	for(i=0; i<16; i++){ 
		if(i%4==0) printf("\n\n");
		if(arr[i]==0) printf(" \t");
		else printf("%d\t", arr[i]);
	}
	printf("\n");
}

BOARD shuffle_state(BOARD prev_board){
	int n=16, i, j, temp;
	srand(time(NULL));
	for(i=15; i>0; i--){
		j=rand()%(i+1);
		temp=prev_board.b[i];
		prev_board.b[i] = prev_board.b[j];
		prev_board.b[j] = temp;
	}
	return prev_board;
}

BOARD generating_goal_state(BOARD goal_state){
	int i;
	for(i=0; i<16; i++){
		goal_state.b[i] = i+1;
		if(i==15) goal_state.b[i] = 0;
	}
	return goal_state;
}

void print_all(NODE *list){
	printf("start printing\n");
	while(list!=NULL){
		print(list->B.b, list->B.board_num, list->B.misplaced_tiles);
		list = list->next;
	}
	printf("printing done\n");
}

int find_zero(int *A){
	int i;
	for(i=0;i<16;i++) if(A[i]==0) return i;
}

int check_if_boards_match(int *a, int *b){
	int count = 0, i, j;
	for(i=0;i<16;i++) if(a[i]==b[i]) count++;
	if(count == 16) return 1;
	else return 0;
}

int getInvCount(int arr[])
{
    int inv_count = 0;
    for (int i = 0; i < N * N - 1; i++){
        for (int j = i + 1; j < N * N; j++){
            // count pairs(i, j) such that i appears
            // before j, but i > j.
            if (arr[j] && arr[i] && arr[i] > arr[j]) inv_count++;
        }
    }
    return inv_count;
}
int findXPosition(int *puzzle)
{
    // start from bottom-right corner of matrix
    for (int i = N - 1; i >= 0; i--)
        for (int j = N - 1; j >= 0; j--)
            if (puzzle[i*4+j] == 0) return N - i;
}
bool isSolvable(int *puzzle)
{
    // Count inversions in given puzzle
    int invCount = getInvCount((int*)puzzle);

    // If grid is odd, return true if inversion
    // count is even.
    if (N & 1) return !(invCount & 1);

    else     // grid is even
    {
        int pos = findXPosition(puzzle);
        if (pos & 1) return !(invCount & 1);
        else return invCount & 1;
    }
}

void copy_into_puzzle(BOARD start_state){
	int i, j;
	for(i=0; i<4; i++){
		for(j=0; j<4; j++) puzzle[i*4+j] = start_state.b[i*4+j];
	}
}

int check_goal_state(int check_board[]){
	int i;
	for(i=0; i<15; i++){
		if(i+1 != check_board[i]) return 0;
	}
	return 1;
}

int find_misplaced_tiles(int *A){
	int i, count_misplaced=0;
	for(i=0; i<15; i++){
		if(A[i] != (i+1)){
			count_misplaced++;
		}
	}
	if(A[15]!=0) count_misplaced++;
	return count_misplaced;
}

void add_solution_boards_to_open_list(int *A){
	NODE *op_list = open_list, *clo_list = close_list;
	int i, flag=0;
	while(op_list!=NULL){
		if(check_if_boards_match(op_list->B.b,A)){
			flag = 1;
			break;
		}
		op_list = op_list->next;
	}
	while(clo_list!=NULL && flag==0){
		if(check_if_boards_match(clo_list->B.b,A)){
			flag = 1;
			break;
		}
		clo_list = clo_list->next;
	}
	if(op_list==NULL && clo_list==NULL){
		flag = 0;
		// no duplicate found. Create new node to insert in open_list
		NODE *new_baccha = (NODE*)malloc(sizeof(NODE));
		new_baccha->next = NULL;
		// updating misplaced_tiles
		int misplaced = find_misplaced_tiles(A);
		new_baccha->B.misplaced_tiles = misplaced;
		new_baccha->B.board_num = board_count++;
		new_baccha->B.f_n = new_baccha->B.misplaced_tiles + new_baccha->B.board_num;
		// updating board
		for(i=0;i<16;i++) new_baccha->B.b[i] = A[i];
		// insert node in the open_list
		if(open_list==NULL) open_list = new_baccha;
		else{
			NODE *current = open_list;
			while (current->next!=NULL && current->B.f_n < new_baccha->B.f_n){
	            current = current->next;
	        }
	        new_baccha->next = current->next;
	        current->next = new_baccha;
    	}
		NODE *t = open_list;
		space_taken+=1;
	}
}

int count_zeros(int *A){
	int i, count=0;
	for(i=0; i<16; i++) if(A[i]==0) count++;
	if(count > 1) return 1;
	else return 0;
}

int repair=0;

void repair_mistake(NODE *op1){
	NODE *op2 = open_list, *fix = open_list;
	while(op1!=NULL){
		int check = count_zeros(op1->B.b);
		if(check){
			if(op1==fix) open_list = open_list->next;
			op2->next = op1->next;
			break;
		}
		op2 = op1;
		op1 = op1->next;
	}
}

void check_possible_solution_boards(int *A){
	int position_of_zero = find_zero(A);
	int row_no = position_of_zero/4;
	int col_no = position_of_zero%4;
	int misplaced_tiles = 0;
	if(row_no-1>=0){
		A[position_of_zero] = A[(row_no-1)*4+col_no];
		A[(row_no-1)*4+col_no] = 0;
		add_solution_boards_to_open_list(A);
		A[(row_no-1)*4+col_no] = A[position_of_zero] ;
		A[position_of_zero] = 0;
	}
	if(col_no-1>=0){
		A[position_of_zero] = A[(row_no)*4+(col_no-1)];
		A[(row_no)*4+(col_no-1)] = 0;
		add_solution_boards_to_open_list(A);
		A[(row_no)*4+(col_no-1)] = A[position_of_zero] ;
		A[position_of_zero] = 0;
	}
	if(row_no+1<=3){
		A[position_of_zero] = A[(row_no+1)*4+col_no];
		A[(row_no+1)*4+col_no] = 0;
		add_solution_boards_to_open_list(A);
		A[(row_no+1)*4+col_no] = A[position_of_zero] ;
		A[position_of_zero] = 0;
	}
	if(col_no+1<=3){
		A[position_of_zero] = A[(row_no)*4+(col_no+1)];
		A[(row_no)*4+(col_no+1)] = 0;
		add_solution_boards_to_open_list(A);
		A[(row_no)*4+(col_no+1)] = A[position_of_zero] ;
		A[position_of_zero] = 0;
	}
	repair++;
	NODE *open_repair = open_list;
	if(repair) repair_mistake(open_repair);
	time_taken+=4;
}

int find_best_possible_solution(){
	// check least misplaced tiles result
	int temp_val = INT_MAX, i;
	NODE *check = open_list, *temp=open_list, *prev=open_list, *curr=open_list, *list=open_list;
	open_list = open_list->next;
	//updating best possible solution
	for(i=0; i<16; i++){
		best_possible_solution[i] = curr->B.b[i];
	}
	// updating open_list by removing best_posssible_solution_board
	// if head is best possible board
	// check if best board = solution
	final_f_n = curr->B.f_n;
	print(best_possible_solution, curr->B.board_num, curr->B.misplaced_tiles);
	int goal=0;
	goal = check_goal_state(best_possible_solution);
	// if yes - add to close_list and terminate
	if(goal){
		if(close_list==NULL) close_list = curr;
		else{
			// inserting at the head
			curr->next = close_list;
			close_list = curr;
		}
		return 1;
	}
	// else add to close list and look for new best possible solutions
	else{ 
		if(close_list==NULL) close_list = curr;
		else{
			// inserting at the head
			curr->next = close_list;
			close_list = curr;
		}
		NODE *temp = close_list;
		return 0;
	}
}

void solve_board(BOARD start_state){
	// check possible solution boards
	// if no duplicate found
	// find out their misplaced tiles
	// add to open_list
	check_possible_solution_boards(start_state.b);
	NODE *op1 = open_list;
	// find best possible solution
	int goal=0;
	goal = find_best_possible_solution();
	// check if goal==1
	// if yes - return answer
	if(goal) return;
	// else start checking possible solution boards in open_list
	else{
		NODE *open_list_head;
		while(open_list!=NULL){
			open_list_head = open_list;
			check_possible_solution_boards(best_possible_solution);
			goal = 0;
			goal = find_best_possible_solution();
			if(goal) return;
		}
	}
}

int main(){
	srand((unsigned int)time(NULL));
	BOARD start_state;
	int i;

	// generating goal(global) and shuffled start state
	goal_state = generating_goal_state(goal_state);

	// manually checking with start_state
	/*start_state.b[0] = 1;
	start_state.b[1] = 2;
	start_state.b[2] = 3;
	start_state.b[3] = 4;
	start_state.b[4] = 5;
	start_state.b[5] = 6;
	start_state.b[6] = 7;
	start_state.b[7] = 8;
	start_state.b[8] = 9;
	start_state.b[9] = 10;
	start_state.b[10] = 11;
	start_state.b[11] = 12;
	start_state.b[12] = 13;
	start_state.b[13] = 0;
	start_state.b[14] = 14;
	start_state.b[15] = 15;*/

	// random generation start_state
	start_state = shuffle_state(goal_state);
	start_state.board_num = 1;
	int m = find_misplaced_tiles(start_state.b);
	start_state.misplaced_tiles = m;
	start_state.f_n = start_state.misplaced_tiles + start_state.board_num;
	goal_state.misplaced_tiles = 0;

	open_list = (NODE*)malloc(sizeof(NODE));
	close_list = (NODE*)malloc(sizeof(NODE));
	open_list->next= NULL;
	close_list->next = NULL;
	// adding start_state to close_list
	close_list->B = start_state;
	printf("Starting board:\n");
	print(start_state.b, start_state.board_num, start_state.misplaced_tiles);
	
	copy_into_puzzle(start_state);
	if(isSolvable(puzzle)) printf("Solvable\n");
	else printf("Unsolvable board state\n");

	if(isSolvable(start_state.b)){
		if(check_goal_state(start_state.b)){
			printf("SOLUTION FOUND\n");
			print(start_state.b, start_state.board_num, start_state.misplaced_tiles);
		}
		else{
			solve_board(start_state);
			printf("\nSolution found for start state:\n");
			print(start_state.b, 1, find_misplaced_tiles(start_state.b));
			printf("\nTotal time (number of nodes generated) = %d\nTotal space (number of nodes present in memory) = %d\n", time_taken, space_taken);
			printf("Best board f(n) = %d\n",final_f_n );
		}
	}

	return 0;
}