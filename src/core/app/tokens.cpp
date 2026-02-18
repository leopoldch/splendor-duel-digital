#include "tokens.h"
#include "card.h"
using namespace std;

int Token::white_number = 0;
int Token::blue_number = 0;
int Token::red_number = 0;
int Token::green_number = 0;
int Token::black_number = 0;
int Token::gold_number = 0;
int Token::perl_number = 0;

vector<const Token *> initTokens() {
	vector<const Token *> token_array;
	int id = 1;

	for (int i = 0; i < 3; ++i) {
		Token *tmp = new Token(id, Color::gold,
		                       "../src/assets/rest_detoured/Jeton_or.png");
		token_array.push_back(tmp);
		id++;
	}
	for (int i = 0; i < 2; ++i) {
		Token *tmp = new Token(id, Color::perl,
		                       "../src/assets/rest_detoured/Jeton_perle.png");
		token_array.push_back(tmp);
		id++;
	}
	for (int i = 0; i < 4; ++i) {
		Token *tmp = new Token(id, Color::red,
		                       "../src/assets/rest_detoured/Jeton_rouge.png");
		token_array.push_back(tmp);
		id++;
	}
	for (int i = 0; i < 4; ++i) {
		Token *tmp = new Token(id, Color::blue,
		                       "../src/assets/rest_detoured/Jeton_bleu.png");
		token_array.push_back(tmp);
		id++;
	}
	for (int i = 0; i < 4; ++i) {
		Token *tmp = new Token(id, Color::green,
		                       "../src/assets/rest_detoured/Jeton_vert.png");
		token_array.push_back(tmp);
		id++;
	}
	for (int i = 0; i < 4; ++i) {
		Token *tmp = new Token(id, Color::black,
		                       "../src/assets/rest_detoured/Jeton_noir.png");
		token_array.push_back(tmp);
		id++;
	}
	for (int i = 0; i < 4; ++i) {
		Token *tmp = new Token(id, Color::white,
		                       "../src/assets/rest_detoured/Jeton_blanc.png");
		token_array.push_back(tmp);
		id++;
	}

	return token_array;
}
