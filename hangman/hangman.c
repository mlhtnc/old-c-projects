#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include "my_wgetch.h"

#define b1 "\u2510"		// sag ust kose
#define b2 "\u2B2F"		// kafa
#define b3 "\u2502"		// dikey cizgi
#define b4 "\u2500"		// yatay cizgi
#define b5 "\u250C"		// sol ust kose
#define b6 "\u2571"		// sag egik cizgi
#define b7 "\u2572"		// sol egik cizgi
#define b8 "\u2534"		// sag sol ayrim

#define gotoyx(y,x) wprintf(L"\033[%d;%dH", y, x)
#define clear() wprintf(L"\033[H\033[J")

/* girilen karakterlerin kontrolunde kullanilan makrolar */
#define isalpha(c) (((c) >= 'A') && ((c) <= 'Z')) || (((c) >= 'a') && ((c) <= 'z'))
#define isalnum(c) (isalpha(c)) || (((c) >= '0') && ((c) <= '9'))
#define isgraph(c) ((c) >= 33) && ((c) <= 126)
#define iscntrl(c) (((c) >= 0) && ((c) <= 31)) || (c==127)
#define ispunct(c) (isgraph(c)) && (!(isalnum(c)))
#define isdigit(c) ((c) >= '0') && ((c) <= '9')

typedef unsigned char boolean;

void ask_word();
void draw_word();
void draw_hangman();
void hang_man();
void draw_alphabet();
void draw_ascii(int,int);
void draw_turkish(int,wchar_t);
void draw_correct_word();
void find_ch();
void clear_str_bool();
void set_position();
void show_cursor(boolean);
wchar_t toupper(wchar_t);
int get_max_word_len(FILE *);
boolean increase_level();
boolean strwcmp();


boolean * word_bool;
wchar_t *word, *word_buf, c;
int max_word_len, next_word_len, line, limit, hang_level;

wchar_t _c = L'ç', _g = L'ğ', _i = L'ı', _o = L'ö', _s = L'ş', _u = L'ü';
wchar_t _C = L'Ç', _G = L'Ğ', _I = L'İ', _O = L'Ö', _S = L'Ş', _U = L'Ü';

int main()
{
	int i = 0;
	FILE * words;

	setlocale(LC_ALL, "");

	// kelime dosyasi okuma icin aciliyor
	words = fopen("words", "r");
	if ( words == NULL )
	{
		wprintf(L"Kelime dosyası bulunamadı.. Program sonlandırılıyor.\n\n");
		exit(0);
	}

	// sozcuklerin tutulacagi dizilerin uzunluklarini
	// belirlemek icin en uzun kelime bulunur
	max_word_len = get_max_word_len(words);

	// bellekten yer ayriliyor
	word = malloc(sizeof(wchar_t) * (max_word_len + 1));
	word_buf = malloc(sizeof(wchar_t) * (max_word_len + 1));
	word_bool = malloc(sizeof(boolean) * 32);		// [0-25] ascii, Ç=26, I=27, Ğ=28, Ö=29, Ş=30, Ü=31

	// cursor gorunmez hale getirilir
	show_cursor(0);

	// words dosyasindan okunan karakterlerin toupper fonksiyonu ile buyuk harf olmasi saglanir
	// her '-' ve '\n' karakteri okundugunda ask_word fonksiyonu cagrilarak siradaki kelime sorulur
	while (( c = toupper( getwc(words) )) != EOF )
	{
		if ( c == '-' || c == 10 )
		{
			next_word_len = i;
			ask_word();

			i = 0;
		}
		else
		{
			word[i] = c;
			i++;
		}
	}

	clear();

	show_cursor(1);
	free(word);
	free(word_buf);
	free(word_bool);
	fclose(words);
	return 0;
}

void ask_word()
{
	c = 0;
	hang_level = 0;

	clear();
	draw_hangman();
	draw_word();
	draw_alphabet();

	while (( c = toupper(getch()) ) != 0 )
	{
		if ( ispunct(c) || iscntrl(c) || isdigit(c) )
			continue;

		clear();
		draw_hangman();
		draw_word();
		draw_alphabet();

		if (strwcmp() == 1)
		{
			gotoyx(18, 42);
			wprintf(L"DOĞRU BİLDİN :)\n");

			getwchar();

			clear_str_bool();
			break;
		}
		else if (hang_level == 7)
		{
			gotoyx(18, 42);
			wprintf(L"KAYBETTİN :(\n");

			sleep(1);
			draw_correct_word();

			getwchar();

			clear_str_bool();
			break;
		}
	}
}

void draw_hangman()
{
	int i;

	line = 10;
	gotoyx(line, 7);
	wprintf(L"%s", b5);

	for(i=0; i < 8; i++)
	{
		wprintf(L"%s", b4);
	}

	wprintf(L"%s",b1);

	gotoyx(++line, 7);
	wprintf(L"%s        %s", b3, b3);

	for(i=0; i < 8; i++)
	{
		gotoyx(++line, 7);
		wprintf(L"%s", b3);
	}

	gotoyx(++line, 5);
	wprintf(L"%s%s%s%s%s", b4, b4, b8, b4, b4);

	if ( increase_level() )
		hang_level++;

	hang_man();
}

void draw_word()
{
	int i;

	limit = 35;
	line = 3;

	gotoyx(line,6);
	for(i=0; i < next_word_len; i++)
	{
		if ( i == limit )
		{
			gotoyx(++line,6);
			limit+=35;
		}

		if (word[i] == c)
		{
			word_buf[i] = c;
			wprintf(L"%lc ", c);
		}
		else if (word_buf[i] != 0)
			wprintf(L"%lc ", word_buf[i]);

		else
		{
			wprintf(L"_ ");
		}
	}
}

void draw_alphabet()
{
	find_ch();

	gotoyx(10,40);
	draw_ascii(0,2);
	draw_turkish(26, _C);
	draw_ascii(3,6);
	draw_turkish(27, _G);
	draw_ascii(7,7);

	gotoyx(11,36);
	draw_ascii(8,8);
	draw_turkish(28, _I);
	draw_ascii(9,14);
	draw_turkish(29, _O);
	draw_ascii(15,18);
	draw_turkish(30, _S);

	gotoyx(12, 42);
	draw_ascii(19,20);
	draw_turkish(31, _U);
	draw_ascii(21,25);
}

void hang_man()
{
	if(hang_level >= 1)
	{
		gotoyx(12, 16);
		wprintf(L"%s", b2);
	}

	if(hang_level >= 2)
	{
		gotoyx(13, 16);
		wprintf(L"%s", b3);
	}

	if(hang_level >= 3)
	{
		gotoyx(13, 15);
		wprintf(L"%s", b6);
	}

	if(hang_level >= 4)
	{
		gotoyx(13, 17);
		wprintf(L"%s", b7);
	}

	if(hang_level >= 5)
	{
		gotoyx(14, 16);
		wprintf(L"%s", b3);
	}

	if(hang_level >= 6)
	{
		gotoyx(15, 15);
		wprintf(L"%s", b6);
	}

	if(hang_level >= 7)
	{
		gotoyx(15, 17);
		wprintf(L"%s", b7);
	}
}

boolean increase_level()
{
	int i;

	if ( c != 0 )
	{
		for(i=0; i < next_word_len; i++)
			if ( word[i] == c )
				return 0;

		if ( c == _C && word_bool[26] == 1 )
			return 0;

		else if ( c == _G && word_bool[27] == 1 )
			return 0;

		else if ( c == _I && word_bool[28] == 1 )
			return 0;

		else if ( c == _O && word_bool[29] == 1 )
			return 0;

		else if ( c == _S && word_bool[30] == 1 )
			return 0;

		else if ( c == _U && word_bool[31] == 1 )
			return 0;

		else if ( word_bool[c-65] == 1 )
			return 0;
		else
			return 1;
	}

	return 0;
}

void draw_ascii(int from, int to)
{
	while( from <= to )
	{
		if ( word_bool[from] == 0 )
			wprintf(L"%c ", from + 65);
		else
			wprintf(L"  ");

		from++;
	}
}

void draw_turkish(int n, wchar_t c)
{
	if ( word_bool[n] == 0 )
		wprintf(L"%lc ", c);
	else
		wprintf(L"  ");
}

void find_ch()
{
	if ( c == _C )
		word_bool[26] = 1;

	else if ( c == _G )
		word_bool[27] = 1;

	else if ( c == _I )
		word_bool[28] = 1;

	else if ( c == _O )
		word_bool[29] = 1;

	else if ( c == _S )
		word_bool[30] = 1;

	else if ( c == _U )
		word_bool[31] = 1;

	else if ( c != 0 )
		word_bool[c-65] = 1;
}

wchar_t toupper(wchar_t c)
{
		if ( c == _c )
			return _C;
		else if ( c == _g )
			return _G;
		else if ( c == _i )
			return 'I';
		else if ( c == 'i' )
			return _I;
		else if ( c == _o )
			return _O;
		else if ( c == _s )
			return _S;
		else if ( c == _u )
			return _U;
		else if (c >= 'a' && c <= 'z')
			return (c - 'a' + 'A');
		else
			return c;
}

void draw_correct_word()
{
	int i;

	line = 3;
	limit = 35;

	gotoyx(line,6);
	for(i=0; i < next_word_len; i++)
	{
		if ( i == limit )
		{
			gotoyx(++line,6);
			limit+=35;
		}

		wprintf(L"%lc ", word[i]);
	}
}

int get_max_word_len(FILE * words)
{
	int length = 0, i = 0;
	wchar_t c;

	while ( ( c=getwc(words) ) != EOF )
	{
		if ( c == '-' || c == 10 )
		{
			if ( length < i )
				length = i;

				i = 0;
		}
		else
			i++;
	}
	rewind(words);

	return length;
}

boolean strwcmp()
{
	int i;

	for(i=0; i < next_word_len; i++)
		if( word[i] != word_buf[i] )
			return 0;

	return 1;
}

void clear_str_bool()
{
	int i;

	for(i=0; i < max_word_len; i++)
	{
		word_buf[i] = 0;
		word[i] = 0;
	}

	for(i=0; i < 32; i++)
		word_bool[i] = 0;
}

void show_cursor(boolean bool)
{
	if(bool)
		wprintf(L"\e[?25h");
	else
		wprintf(L"\e[?25l");
}

