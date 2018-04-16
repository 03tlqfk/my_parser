#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


char token;

int state = 0;				
char stack[200] = { 0, };	
int count_s = 0;			//stack location
char x[200] = { 0, };		//state stack
int count_x = 0;			//state stack location
char buffer[200] = { 0, };	
int count_b = 0;			//buffer loaction


int exp();
int term();
int factor();

//parser table
char a_table[2][17][10] =	
{
	{
		{ NULL, NULL, NULL, NULL, NULL, 's', NULL, 's', 's', NULL },
{ NULL, 's', 's', NULL, NULL, NULL, NULL, NULL, NULL, 'a' },
{ NULL, 'r', 'r', 's', 's', NULL, 'r', NULL, NULL, 'r' },
{ NULL,  'r', 'r', 'r', 'r', NULL, 'r', NULL, NULL, 'r' },
{ NULL, NULL, NULL, NULL, NULL, 's', NULL, 's', 's', NULL },
{ NULL, 'r', 'r', 'r', 'r', NULL, 'r', NULL, NULL, 'r' },
{ NULL, 'r', 'r', 'r', 'r', NULL, 'r', NULL, NULL, 'r' },
{ NULL, NULL, NULL, NULL, NULL, 's', NULL, 's', 's', NULL },
{ NULL, NULL, NULL, NULL, NULL, 's', NULL, 's', 's', NULL },
{ NULL, NULL, NULL, NULL, NULL, 's', NULL, 's', 's', NULL },
{ NULL, NULL, NULL, NULL, NULL, 's', NULL, 's', 's', NULL },
{ NULL, 's', 's', NULL, NULL, NULL, 's', NULL, NULL, NULL },
{ NULL, 'r', 'r', 's', 's', NULL, 'r', NULL, NULL, 'r' },
{ NULL, 'r', 'r', 's', 's', NULL, 'r', NULL, NULL, 'r' },
{ NULL, 'r', 'r', 'r', 'r', NULL, 'r', NULL, NULL, 'r' },
{ NULL, 'r', 'r', 'r', 'r', NULL, 'r', NULL, NULL, 'r' },
{ NULL, 'r', 'r', 'r', 'r', NULL, 'r', NULL, NULL, 'r' },
	},
	{
		{ NULL,  NULL, NULL, NULL, NULL, '4', NULL, '5', '6', NULL },
{ NULL, '7', '8', NULL, NULL, NULL, NULL, NULL, NULL, 'a' },
{ NULL, '3', '3', '9', '10', NULL, '3', NULL, NULL, '3' },
{ NULL,  '6', '6', '6', '6', NULL, '6', NULL, NULL, '6' },
{ NULL,  NULL, NULL, NULL, NULL, '4', NULL, '5', '6', NULL },
{ NULL,  '8', '8', '8', '8', NULL, '8', NULL, NULL, '8' },
{ NULL,  '9', '9', '9', '9', NULL, '9', NULL, NULL, '9' },
{ NULL,  NULL, NULL, NULL, NULL, '4', NULL, '5', '6', NULL },
{ NULL,  NULL, NULL, NULL, NULL, '4', NULL, '5', '6', NULL },
{ NULL,  NULL, NULL, NULL, NULL, '4', NULL, '5', '6', NULL },
{ NULL,  NULL, NULL, NULL, NULL, '4', NULL, '5', '6', NULL },
{ NULL, '7', '8', NULL, NULL, NULL, '16', NULL, NULL, NULL },
{ NULL, '1', '1', '9', '9', NULL, '1', NULL, NULL, '1' },
{ NULL,  '2', '2', '10', '10', NULL, '2', NULL, NULL, '2' },
{ NULL, '4', '4', '4', '4', NULL, '4', NULL, NULL, '4' },
{ NULL, '5', '5', '5', '5', NULL, '5', NULL, NULL, '5' },
{ NULL, '7', '7', '7', '7', NULL, '7', NULL, NULL, '7' }
	}
};

char g_table[17][4] =	//goto table
{
	{ NULL,  '1','2','3' },
{ NULL, NULL, NULL, NULL },
{ NULL, NULL, NULL, NULL },
{ NULL, NULL, NULL, NULL },
{ NULL,  '11', '2','3' },
{ NULL,  NULL, NULL, NULL },
{ NULL,  NULL, NULL, NULL },
{ NULL,  NULL, '12', '3' },
{ NULL,  NULL, '13' , '3' },
{ NULL,  NULL, NULL, '14' },
{ NULL,  NULL, NULL, '15' },
{ NULL,  NULL, NULL, NULL },
{ NULL,  NULL, NULL, NULL },
{ NULL,  NULL, NULL, NULL },
{ NULL,  NULL, NULL, NULL },
{ NULL,  NULL, NULL, NULL },
{ NULL,  NULL, NULL, NULL }
};

char r_table[10][3]
{
	{ NULL,  NULL, NULL },
{ 3,1,'E' },
{ 3,1,'E' },
{ 1,1,'E' },
{ 3,1,'T' },
{ 3,1,'T' },
{ 1,1,'T' },
{ 3,1,'F' },
{ 1,1,'F' },
{ 1,1,'F' },
};

//pop push
void pop_s()
{
	stack[count_s - 1] = NULL;
	count_s--;	
}
void push_s(char ch)
{
	stack[count_s] = ch;
	count_s++;	
}
void pop_x()
{
	x[count_x - 1] = NULL;
	count_x--;
}
void push_x(char ch)
{
	x[count_x] = ch;
	count_x++;
}

void print()
{
	int i, j;
	printf("\tstack : 0");
	for (i = 0; i < count_s; i++)
	{
		printf("%c%c", stack[i], x[i]);
	}
	printf("\t\tbuffer : ");
	for (j = count_b; j < strlen(buffer); j++)
	{
		printf("%c", buffer[j]);
	}


	printf("\n");

}

void gotoo() 
{
	char non_t = stack[count_s - 1];
	int temp_g = 0;
	if (non_t == 'F')	//3,14,15
	{
		//T*F
		if (stack[count_s - 3] == 'T' &&stack[count_s - 2] == '3' &&stack[count_s - 1] == 'F')
		{
			temp_g = 14;
			pop_x();
			push_x('14');
		}
		//T/F
		else if (stack[count_s - 3] == 'T' &&stack[count_s - 2] == '4' &&stack[count_s - 1] == 'F')
		{
			temp_g = 15;
			pop_x();
			push_x('15');
		}
		else
		{
			temp_g = 3;
			pop_x();
			push_x('3');
		}
	}
	else if (non_t == 'T')	//2,12,13
	{
		//E+T
		if (stack[count_s - 3] == 'E' &&stack[count_s - 2] == '1' &&stack[count_s - 1] == 'T')
		{
			temp_g = 12;
			pop_x();
			push_x('12');
		}
		//E-T
		else if (stack[count_s - 3] == 'E' &&stack[count_s - 2] == '2' &&stack[count_s - 1] == 'T')
		{
			temp_g = 13;
			pop_x();
			push_x('13');
		}
		else
		{
			temp_g = 2;
			pop_x();
			push_x('2');
		}

	}
	else if (non_t == 'E')	//1,11
	{
		//(E)
		if (stack[count_s - 2] == '5' && stack[count_s - 1] == 'E')
		{
			temp_g = 11;
			pop_x();
			push_x('11');
		}
		else
		{
			temp_g = 1;
			pop_x();
			push_x('1');
		}
	}

	state = temp_g; //state change
	printf("goto%d", temp_g);

}

void shift(int stat, int act)	//move terminal
{
	char temp_s = a_table[1][stat][act];
	push_s(buffer[count_b]);
	count_b++;	
	push_x(temp_s);	

	if (state == 11 && act == 6)
	{
		state = 16;
	}
	else if ((state == 2 && act == 4) || (state == 12 && act == 4) || (state == 13 && act == 4))
	{
		state = 10;
	}

	else
		state = (int)temp_s - 48;	//change state

	printf("shift%d", state);
}

void reduce(int stat, int act)	//terminal->non terminal  or  non terminal->non terminal
{
	char temp_r = a_table[1][stat][act];
	int num_r = (int)a_table[1][stat][act] - 48;		
														
	for (int i = 0; i < r_table[num_r][0]; i++)
	{
		pop_s();
		pop_x();
	}

	push_s(r_table[num_r][2]);

	push_x(temp_r);	
	state = num_r;	//state change
	printf("reduce%c", temp_r);


}



void parser()
{


	while (1)	
	{
		int action = (int)buffer[count_b] - 48;	
		if (a_table[0][state][action] == 's')
		{
			shift(state, action);
			print();
			action = (int)buffer[count_b] - 48;
		}
		else if (a_table[0][state][action] == 'r')
		{
			reduce(state, action);
			print();
			action = (int)buffer[count_b] - 48;
			if (a_table[0][state][action] == 'a')
			{
				printf("acc");
				print();
				break;
			}
			else if (stack[count_s - 1] == 'E' || stack[count_s - 1] == 'T' || stack[count_s - 1] == 'F')
			{
				gotoo();
				print();
				action = (int)buffer[count_b] - 48;
			}
		}
		else if (a_table[0][state][action] == 'a')
		{
			printf("acc");
			print();
			break;
		}
	}
}

void error()
{
	fprintf(stderr, "error\n");
	exit(1);
}

void match(char inputstring)
{
	if (token == inputstring)
	{
		token = getchar();
	}
	else error();
}

int main()
{
	int flow;
	token = getchar();
	flow = exp();
	strcat_s(buffer, INT_MAX, "9");
	printf("\n%s\n", buffer);

	print();
	parser();

	return 0;
}



int exp()
{
	int temp = term();
	while ((token == '+') || (token == '-'))
		switch (token)
		{
		case '+':	//+
			printf("1 ");
			strcat_s(buffer, INT_MAX, "1");
			match('+');
			temp += term();
			break;
		case '-':	//-
			printf("2 ");
			strcat_s(buffer, INT_MAX, "2");
			match('-');
			temp -= term();
			break;
		}
	return temp;
}

int term()
{
	int temp = factor();
	while ((token == '*') || (token == '/'))
		switch (token)
		{
		case '*':	//*
			printf("3 ");
			strcat_s(buffer, INT_MAX, "3");
			match('*');
			temp *= term();
			break;
		case '/':	// /
			printf("4 ");
			strcat_s(buffer, INT_MAX, "4");
			match('/');
			temp /= term();
			break;
		}
	return temp;
}
int factor()
{
	int temp;
	char symbol;
	if (token == '(')
	{
		printf("5 ");	//(
		strcat_s(buffer, INT_MAX, "5");
		match('(');
		temp = exp();
		printf("6 ");	//)
		strcat_s(buffer, INT_MAX, "6");
		match(')');
	}
	else if (isalpha(token))	//id
	{
		printf("7 ");
		strcat_s(buffer, INT_MAX, "7");
		ungetc(token, stdin);
		scanf_s("%c", &temp);
		token = getchar();
		while (token != '+' && token != '-' && token != '*' && token != '/' && token != '\n' && token != '(' && token != ')')
		{
			if (isalpha(token))
			{
				ungetc(token, stdin);
				scanf_s("%c", &temp);
				token = getchar();
			}
			else if (isdigit(token))
			{
				ungetc(token, stdin);
				scanf_s("%d", &temp);
				token = getchar();
			}
		}
	}
	else if (isdigit(token))	//num
	{
		printf("8 ");
		strcat_s(buffer, INT_MAX, "8");
		ungetc(token, stdin);
		scanf_s("%d", &temp);
		token = getchar();
	}
	else error();
	return temp;
}



