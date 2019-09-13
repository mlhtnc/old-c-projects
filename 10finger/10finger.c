#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include "my_wgetch.h"

// hatali ve varsayilan harf rengi makrolari tanimlamalari
#define DEFAULT	"\e[0m"
#define RED		"\e[31m"

#define _isprint(c) ((c) >= 32) && ((c) <= 126)

void get_accuracy(wchar_t *, wchar_t *);
void read_text(FILE *, wchar_t *);
void print_text(wchar_t *);
int get_size(FILE *);
double get_delta(struct timespec);

int	correct_letter = 0, wrong_letter = 0, str_len, text_lines = 0, line = 1, * column, cpm, i;

int main()
{
	FILE	*text;
	struct	timespec start;
	double	delta;
	wchar_t	c, *str, *str_buf;
	
	column = malloc(sizeof(int));

	// yerel C yerelinden sistem yereline donusturulur
	setlocale(LC_ALL, "");
	
	// ekran temizleniyor
	system("clear");
	
	// dosya okuma icin acilir, yoksa hata mesaji dondurulur
	text = fopen("text", "r");
	if ( text == NULL )
	{
		wprintf(L"Metin dosyası bulunamadı.. Program sonlandırılıyor.\n"
					"""text"" adında bir dosya oluşturun.\n\n");
		exit(0);
	}
	
	// kac karakter okunacagi ogrenilir, eğer karakter yoksa hata mesaji dondurulur
	str_len = get_size(text) - 1;
	if ( str_len == -1)
	{
		wprintf(L"Metin dosyası boş.. Program sonlandırılıyor.\n\n");
		exit(0);
	}

	// bellekten yer ayriliyor
	str = malloc(sizeof(wchar_t) * (str_len + 1));
	str_buf = malloc(sizeof(wchar_t) * (str_len + 1));
	
	// dosyadan str_len uzunlugunda karakter okunur ve kontrol icin str dizisine kopyalanir
	rewind(text);
	read_text(text, str);
	
	// yazilacak metin ekrana basilir
	print_text(str);	
	
	i = -1;
	wprintf(L">");
	do
	{
		c = getch();
		
		if(i==0)
			i=-1;		
		
		if ( i == -1)
		{
			clock_gettime(CLOCK_MONOTONIC, &start);
			i=0;
		}	
		
		if ( c != 127 && c != 9 )
		{
			if(c == str[i])
			{
				wprintf(L"%lc", c);
			}
			else
			{
				wprintf(L"%s%lc%s", RED, c, DEFAULT);
			}
			
			if(c == 10)
			{
				wprintf(L">");
				line++;
				
				column = realloc(column, sizeof(int) * line);
				column[line-1] = 0;
			}
			else
				column[line-1]++;
			
			str_buf[i] = c;
			i++;
		}
		else if( i != 0 && c != 9 )
		{
			if ( str_buf[i-1] == 10 )
			{
				line--;
				wprintf(L"\b \b");
				wprintf(L"\033[%d;%dH", line + text_lines, column[line-1] + 2);
				column = realloc(column, sizeof(int) * line);
			}
			else
			{
				wprintf(L"\b \b");
				column[line-1]--;
			}
			i--;
		}
		
	} while(i != str_len);
	
	get_accuracy(str, str_buf);
	delta = get_delta(start);
	cpm = correct_letter * (60 / delta) + 0.5f;
	
	free(str);
	free(str_buf);
	free(column);
	
	wprintf(L"\n\n=====================\n");
	wprintf(L"WPM: %d\n", (int) ((cpm / 5.0f) + 0.5f));
	wprintf(L"CPM: %d\n", cpm);
	//wprintf(L"WPM: %d\n", (int) ((correct_letter / 5) * (60 / (double) delta)));
	//wprintf(L"CPM: %d\n", (int) (correct_letter * (60 / (double) delta)));
	wprintf(L"Gecen sure: %.3lf\n", delta);
	wprintf(L"Dogru harf: %d\nYanlis harf: %d\n", correct_letter, wrong_letter);
	wprintf(L"=====================\n\n");
	
	fclose(text);
	return 0;
}

void print_text(wchar_t * str)
{
	i=0;
	while(str[i] != 0)
	{
		if (str[i] == 10)
		{
			wprintf(L"%lc", str[i]);
			wprintf(L">");
		}
		else if (i == 0)
		{
			wprintf(L">");
			wprintf(L"%lc", str[i]);
		}
		else
			wprintf(L"%lc", str[i]);
		
		i++;
	}
	
	wprintf(L"\n\n");
}

int get_size(FILE * text)
{
	i=0;
	while(getwc(text) != EOF)	i++;
	
	return i;
}

void get_accuracy(wchar_t * str, wchar_t * str_buf)
{
	for(i=0; i < str_len; i++)
	{
		if(str[i] == str_buf[i])
			correct_letter++;
		else
			wrong_letter++;
	}
}

void read_text(FILE * fp, wchar_t *str)
{
	for(i=0; i < str_len; i++)
	{
		str[i] = getwc(fp);
		
		if( str[i] == 10 )
			text_lines++;
	}
	text_lines += 2;
}

double get_delta(struct timespec start)
{
	struct timespec stop;
	
	clock_gettime(CLOCK_MONOTONIC, &stop);
	
	double delta_sec = (double) stop.tv_sec - start.tv_sec;
	double delta_nsec = ( (double) stop.tv_nsec / 1000000000 ) - ( (double) start.tv_nsec / 1000000000 );
	double delta = delta_sec + delta_nsec;

	return delta ;
}

