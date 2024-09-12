#include <chrono>
#include <thread>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <cstring>

//#define TEST_SPEED

using namespace std;

enum BOTS_DEPTH {
	EASY_COMPLEXITY = 1,
	HARD_COMPLEXITY = 5
};

enum COMPLEXITY_LEVEL {
	NOT_SELECTED = 0,
	HUMAN_WITH_BOT = 1,
	EASY_LEVEL = 11,
	HARD_LEVEL = 12,
	BOT_WITH_BOT = 3,
	QUIT_GAME = 4,
	BOT_WITH_BOT_EASY = 31,
	BOT_WITH_BOT_HARD = 32,
	FREE_MODE = 5
};

enum CHECKERS {
	NO_ONE = 0,
	FIRST_CHECKER = 1,
	SECOND_CHECKER = 2,
	FIRST_CHECKER_D = 11,
	SECOND_CHECKER_D = 22,
};

enum MOVES {
	NO_MOVES = 0,
	ORDINARY_MOVE = 1,
	FIGHTING_MOVE = 2
};

struct Coordinates {
	char x = -1;
	char y = -1;
};

struct draw {
	int moves_without_changes = 0;
	char number_of_checkers = 0;
	long long last_weight = 0;

	int checker_moved = 0;
} draw;

struct moves_list_node {
	Coordinates cell_from, cell_to;

	long long weight;

	moves_list_node* p_next;
};

class Desk {
public:
	~Desk();

	char NoCheckers();
	char TryToOrdinaryMove(char side);
	char TryToFightingMove(char side);
	char AvaliableMovesForAllCheckers(char side);
	void SelectChecker();
	char AvaliableMovesForOneCheckers(char x, char y, char side);
	void AddMovesForChecker(moves_list_node** list_begin, char x, char y, char side, char avaliable_move);
	void GenerateMoves(moves_list_node** list_begin, char avaliable_move, char side);
	void DoMove(char depth, char side_to_move, char bots_side, char avaliable_move, long long alpha, long long beta);
	long long EstimateMoves(moves_list_node* list_begin, char depth, char side_to_move, char bots_side, char avaliable_move, long long alpha, long long beta);
	void BotsMove(char side_to_move, char bots_side);
	void HumansMove();
	void EndOfGame(char& game);
	void ClearDeskStatesList(void);
	void AddStateToList();

	void SetDefault();
	void SetCurrentMove(char side);
	void SetDepth(char depth);
	void SetSideSelected(char side);
	void SetChecker(Coordinates& coords, char checker);
	void SetAvaliableMove();

	char GetSideSelected();
	char GetResult();
	char GetCurrentMove();


	void PrintDeskInConsole() {
		for (char i = 0; i < 8; i++) {
			cout << endl << "\r" << 8 - i << "  ";
			for (char j = 0; j < 8; j++) {
				cout << (int)desk[i * 8 + j] << setw(3) << setfill(' ');
			}
		}

		cout << endl << "\r   ";
		for (char i = 0; i < 8; i++)
			cout << (char)('A' + i) << setw(3) << setfill(' ');
		cout << endl << "  ";
	}

private:
	long long weight_for_first = 0;
	char avaliable_move = ORDINARY_MOVE; //The variable stores the type of move that can be made (any move or a move with a fight)
	char chain_fight = 0; //Fighting several checkers in one move
	char current_move = FIRST_CHECKER; //Initially, the first player has a move, his checkers are white
	char depth = 1;
	char result = NO_ONE; //The result of the game
	char side_selected = FIRST_CHECKER;

	Coordinates cell_from, cell_to; //Indices of the checker being moved before and after the move

	char desk[64] = { NO_ONE };

	// Field with checkers - 1 - checker of the first player; 11 - queen of the first player;
	// 2 - checker of the second player; 22 - queen of the second player;
	// 0 - empty field
	//
	//Initial: (Set in Desk::SetDefault)
	//0 2 0 2 0 2 0 2
	//2 0 2 0 2 0 2 0
	//0 2 0 2 0 2 0 2
	//0 0 0 0 0 0 0 0
	//0 0 0 0 0 0 0 0
	//1 0 1 0 1 0 1 0
	//0 1 0 1 0 1 0 1
	//1 0 1 0 1 0 1 0
	//1 0 1 0 1 0 1 0

	struct desk_state {
		char desk[64];
		char count;
		struct desk_state* p_next;
	};

	desk_state* desk_states_list = nullptr;
	char max_repetitation = -1;
};

void CoordinatesToIndexes(Coordinates& coords_from, Coordinates& coords_to);

void AddNode(moves_list_node** list_begin, const char x_from, const char y_from, const char x_to, const char y_to);

void DeleteList(moves_list_node* list_begin);
void SelectMove(moves_list_node* list_begin, Coordinates& cell_from, Coordinates& cell_to);

char GameHumanWithBot(Desk& desk);
char GameBotWithBot(Desk& desk);
char EndGameMenu(Desk& desk, char result);

int randint(int nMin, int nMax) {
	return nMin + (int)((double)rand() / (RAND_MAX + 1) * (nMax - nMin + 1));
}

#ifdef TEST_SPEED			// CALCULATE GAME TIME
double start_time = 0, end_time = 0, spent_time = 0, all_time = 0;
int count_moves = 0, count_all_moves;
#endif

int main() {
	srand(15656);

	while (1) {
		Desk desk;

		int difficulty_level = 0; //Difficulty level: 11 - Easy; 12 - difficult; 31, 32 - two bots of the corresponding difficulty play;

#ifdef TEST_SPEED
		difficulty_level = BOT_WITH_BOT_HARD;
#else
		cout << "Select mode:" << endl
			<< "Play with bot - 1." << endl
			<< "Bot with bot - 2." << endl
			<< "exit - 3." << endl
			<< ": ";
		do {
			cout << "\r: ";
			cin >> difficulty_level;
			cout << difficulty_level;
		} while (1 > difficulty_level || 3 < difficulty_level);

		cout << endl;

		if (difficulty_level == 3) {
			difficulty_level = QUIT_GAME;
			return 0;
		}

		if (difficulty_level == 1)
			difficulty_level = HUMAN_WITH_BOT;

		if (difficulty_level == 2)
			difficulty_level = BOT_WITH_BOT;


		if (difficulty_level == BOT_WITH_BOT || difficulty_level == HUMAN_WITH_BOT) {
			cout << "Select difficulty:" << endl
				<< "Easy - 1." << endl
				<< "Hard - 2." << endl
				<< "Exit - 3." << endl
				<< ": ";

			int diff = 0;
			do {
				cout << "\r: ";
				cin >> diff;
			} while (1 > diff || 3 < diff);

			cout << endl;

			if (diff == 3) {
				diff = QUIT_GAME;
				return 0;
			}

			if (difficulty_level == BOT_WITH_BOT)
				if (diff == 1)
					difficulty_level = BOT_WITH_BOT_EASY;
				else
					difficulty_level = BOT_WITH_BOT_HARD;
			else
				if (diff == 1)
					difficulty_level = EASY_LEVEL;
				else
					difficulty_level = HARD_LEVEL;

			if (difficulty_level == EASY_LEVEL || difficulty_level == HARD_LEVEL) {
				cout << "Select color:" << endl
					<< "White - 1." << endl
					<< "Black - 2." << endl
					<< "Exit - 3." << endl
					<< ": ";

				int diff = 0;
				do {
					cout << "\r: ";
					cin >> diff;
				} while (1 > diff || 3 < diff);

				cout << endl;

				if (diff == 3) {
					diff = QUIT_GAME;
					return 0;
				}
				if (diff == 1)
					desk.SetSideSelected(FIRST_CHECKER);
				else
					desk.SetSideSelected(SECOND_CHECKER);

			}
		}
#endif

		//Difficulty selection
		if (difficulty_level == EASY_LEVEL || difficulty_level == BOT_WITH_BOT_EASY)
			desk.SetDepth(EASY_COMPLEXITY);
		else
			desk.SetDepth(HARD_COMPLEXITY);

		if (desk.GetSideSelected() == SECOND_CHECKER) //If the player chose black, then the sprites swap places, and the turn immediately goes to the second player (with white checkers)
			desk.SetCurrentMove(SECOND_CHECKER);

		if (difficulty_level == EASY_LEVEL || difficulty_level == HARD_LEVEL) {
		//Playing a human with a bot
			desk.SetDefault();

			desk.PrintDeskInConsole();

			if (GameHumanWithBot(desk))
				return 0;

			if (EndGameMenu(desk, desk.GetResult()))
				return 0;
		}

		if (difficulty_level == BOT_WITH_BOT_EASY || difficulty_level == BOT_WITH_BOT_HARD) {

#ifdef TEST_SPEED
			int res[3] = { 0 };
			for (int i = 1; i <= 300; i++) {
				//Bot-to-bot game
				desk.SetDefault();

				count_moves = 0;

				start_time = clock();

				if (GameBotWithBot(desk))
					return 0;

				end_time = clock();
				spent_time = (end_time - start_time) / CLOCKS_PER_SEC;

				all_time += spent_time;
				count_all_moves += count_moves;

				if (desk.GetResult() == FIRST_CHECKER)
					res[1]++;
				if (desk.GetResult() == SECOND_CHECKER)
					res[2]++;
				if (desk.GetResult() == NO_ONE)
					res[0]++;

				system("cls");
				cout << "Test number " << i << "..." << endl
					<< "White: " << res[1] << endl
					<< "Black: " << res[2] << endl
					<< "Draw: " << res[0];

				cout << endl << endl << "Time spent on game: " << spent_time << " seconds." << endl
					<< "Move time: " << spent_time / count_moves << " seconds." << endl << endl
					<< "Average game time: " << all_time / i << " seconds." << endl
					<< "Average move time: " << all_time / count_all_moves << " seconds.";
			}
			return 0;
#else
			//Bot-to-bot game
			desk.SetDefault();

			cout << endl << "1 bots move:";
			desk.PrintDeskInConsole();

			if (GameBotWithBot(desk))
				return 0;

			if (EndGameMenu(desk, desk.GetResult()))
				return 0;
#endif
		}
		desk.ClearDeskStatesList(); //Clearing the board state list
	}
}

char GameHumanWithBot(Desk& desk) {
	char game = 1;
	while (game) {
		draw.checker_moved = 0;

		if (desk.GetCurrentMove() == FIRST_CHECKER)
			desk.HumansMove();
		else
			desk.BotsMove(SECOND_CHECKER, SECOND_CHECKER);


		if (desk.GetCurrentMove() == FIRST_CHECKER)
			cout << endl << "Your move:";
		else
			cout << endl << "Bots move:";
		desk.PrintDeskInConsole();

		desk.EndOfGame(game);
	}
	return 0;
}

char GameBotWithBot(Desk& desk) {
	char game = 1;
	while (game) {
		draw.checker_moved = 0;

		if (desk.GetCurrentMove() == FIRST_CHECKER) {
			desk.BotsMove(FIRST_CHECKER, FIRST_CHECKER);
#ifdef TEST_SPEED			// CALCULATE GAME TIME
			count_moves++;
#endif
		}

		else {
			desk.BotsMove(SECOND_CHECKER, SECOND_CHECKER);
#ifdef TEST_SPEED			// CALCULATE GAME TIME
			count_moves++;
#endif
		}
#ifndef TEST_SPEED
		if (desk.GetCurrentMove() == FIRST_CHECKER)
			cout << endl << "1 bots move:";
		else
			cout << endl << "2 bots move:";

		desk.PrintDeskInConsole();
#endif
		desk.EndOfGame(game);
	}
	return 0;
}

void Desk::EndOfGame(char& game) {
	if (NoCheckers() || AvaliableMovesForAllCheckers(GetCurrentMove()) == NO_MOVES) {
		if (((NoCheckers() == FIRST_CHECKER) || (AvaliableMovesForAllCheckers(SECOND_CHECKER) == NO_MOVES)) != (side_selected == SECOND_CHECKER))
			result = FIRST_CHECKER;
		else
			result = SECOND_CHECKER;

		game = 0;
	}

	if (max_repetitation == 3) { //The threefold repetitation
		result = NO_ONE;
		game = 0;
	}

	if (draw.checker_moved) {
		draw.number_of_checkers = 0;
		char* desk_ptr = &desk[0];
		while (desk_ptr++ != &desk[63])
			if (*desk_ptr)
				draw.number_of_checkers++;


		if (draw.last_weight == weight_for_first) {
			draw.moves_without_changes++;
			if ((draw.number_of_checkers == 2 || draw.number_of_checkers == 3) && draw.moves_without_changes >= 5 ||
				(draw.number_of_checkers == 4 || draw.number_of_checkers == 5) && draw.moves_without_changes >= 30 ||
				(draw.number_of_checkers == 6 || draw.number_of_checkers == 7) && draw.moves_without_changes >= 60) {

				result = NO_ONE;////////////////////////////////////////////////////////////////////////////////
				game = 0;////////////////////////////////////////////////////////////////////////////////
			}
		}
		else {
			draw.moves_without_changes = 0;
			draw.last_weight = weight_for_first;
		}
	}
}

char EndGameMenu(Desk& desk, char result) {
	string result_text;

	switch (result) {
	case NO_ONE:
		result_text = "Draw.";
		break;
	case FIRST_CHECKER:
		result_text = "White wins.";
		break;
	case SECOND_CHECKER:
		result_text = "Black wins.";
		break;
	}

	cout << "\r" << result_text << endl << endl;
	cout << "Action:" << endl
		<< "Menu - 1." << endl
		<< "Exit - 2." << endl
		<< ": ";

	int diff = 0;
	do {
		cout << "\r: ";
		cin >> diff;
	} while (1 > diff || 2 < diff);

	cout << endl;

	if (diff == 2)
		return -1;


	return 0;
}

void AddNode(moves_list_node** list_begin, const char x_from, const char y_from, const char x_to, const char y_to) {
	moves_list_node* new_node = new moves_list_node;

	new_node->cell_from.x = x_from;
	new_node->cell_from.y = y_from;
	new_node->cell_to.x = x_to;
	new_node->cell_to.y = y_to;
	new_node->weight = 0;

	new_node->p_next = *list_begin;
	(*list_begin) = new_node;
}

void DeleteList(moves_list_node* list_begin) {
	moves_list_node* tmp = list_begin;
	while (tmp != nullptr) {
		list_begin = list_begin->p_next;
		delete tmp;
		tmp = list_begin;
	}
}

void Desk::GenerateMoves(moves_list_node** list_begin, char avaliable_move, char side) {
	for (char x = 0; x < 8; x++) {
		for (char y = x & 1 ^ 1; y < 8; y += 2) {
			if (desk[x * 8 + y] % 10 == side) {
				AddMovesForChecker(list_begin, x, y, side, avaliable_move);
			}
		}
	}
}

void Desk::AddMovesForChecker(moves_list_node** list_begin, char x, char y, char side, char avaliable_move) {
	char opposite_side = (FIRST_CHECKER + SECOND_CHECKER) - side;

	if (avaliable_move == FIGHTING_MOVE) {
		if (desk[x * 8 + y] == FIRST_CHECKER || desk[x * 8 + y] == SECOND_CHECKER) {
			char diff = (desk[x * 8 + y] == FIRST_CHECKER) ? -1 : 1;

			if ((unsigned char)(x + (diff << 1)) < 8 && y + 2 < 8 && !desk[(x + (diff << 1)) * 8 + y + 2] && desk[(x + diff) * 8 + y + 1] % 10 == opposite_side)
				AddNode(list_begin, x, y, x + (diff << 1), y + 2);

			if ((unsigned char)(x + (diff << 1)) < 8 && y - 2 > -1 && !desk[(x + (diff << 1)) * 8 + y - 2] && desk[(x + diff) * 8 + y - 1] % 10 == opposite_side)
				AddNode(list_begin, x, y, x + (diff << 1), y - 2);

			if ((unsigned char)(x - (diff << 1)) < 8 && y + 2 < 8 && !desk[(x - (diff << 1)) * 8 + y + 2] && desk[(x - diff) * 8 + y + 1] % 10 == opposite_side)
				AddNode(list_begin, x, y, x - (diff << 1), y + 2);

			if ((unsigned char)(x - (diff << 1)) < 8 && y - 2 > -1 && !desk[(x - (diff << 1)) * 8 + y - 2] && desk[(x - diff) * 8 + y - 1] % 10 == opposite_side)
				AddNode(list_begin, x, y, x - (diff << 1), y - 2);
		}
		else {
			char opponents_checkers = 0;

			for (char iter_x = x + 1, iter_y = y + 1; iter_x < 8 && iter_y < 8; iter_x++, iter_y++) {
				if (desk[iter_x * 8 + iter_y] % 10 == side) break;
				if (desk[iter_x * 8 + iter_y] % 10 == opposite_side) opponents_checkers++;
				if (opponents_checkers > 1) break;
				if (!desk[iter_x * 8 + iter_y] && opponents_checkers)
					AddNode(list_begin, x, y, iter_x, iter_y);
			}

			opponents_checkers = 0;
			for (char iter_x = x - 1, iter_y = y + 1; iter_x > -1 && iter_y < 8; iter_x--, iter_y++) {
				if (desk[iter_x * 8 + iter_y] % 10 == side) break;
				if (desk[iter_x * 8 + iter_y] % 10 == opposite_side) opponents_checkers++;
				if (opponents_checkers > 1) break;
				if (!desk[iter_x * 8 + iter_y] && opponents_checkers)
					AddNode(list_begin, x, y, iter_x, iter_y);
			}

			opponents_checkers = 0;
			for (char iter_x = x + 1, iter_y = y - 1; iter_x < 8 && iter_y > -1; iter_x++, iter_y--) {
				if (desk[iter_x * 8 + iter_y] % 10 == side) break;
				if (desk[iter_x * 8 + iter_y] % 10 == opposite_side) opponents_checkers++;
				if (opponents_checkers > 1) break;
				if (!desk[iter_x * 8 + iter_y] && opponents_checkers)
					AddNode(list_begin, x, y, iter_x, iter_y);
			}

			opponents_checkers = 0;
			for (char iter_x = x - 1, iter_y = y - 1; iter_x > -1 && iter_y > -1; iter_x--, iter_y--) {
				if (desk[iter_x * 8 + iter_y] % 10 == side) break;
				if (desk[iter_x * 8 + iter_y] % 10 == opposite_side) opponents_checkers++;
				if (opponents_checkers > 1) break;
				if (!desk[iter_x * 8 + iter_y] && opponents_checkers)
					AddNode(list_begin, x, y, iter_x, iter_y);
			}
		}
	}
	else {
		if (desk[x * 8 + y] == FIRST_CHECKER || desk[x * 8 + y] == SECOND_CHECKER) {
			char diff = (desk[x * 8 + y] == FIRST_CHECKER) ? -1 : 1;

			if (x + diff > -1 && x + diff < 8 && y + 1 < 8 && !desk[(x + diff) * 8 + y + 1])
				AddNode(list_begin, x, y, x + diff, y + 1);

			if (x + diff > -1 && x + diff < 8 && y - 1 > -1 && !desk[(x + diff) * 8 + y - 1])
				AddNode(list_begin, x, y, x + diff, y - 1);
		}
		else {
			for (char iter_x = x + 1, iter_y = y + 1; iter_x < 8 && iter_y < 8; iter_x++, iter_y++) {
				if (!desk[iter_x * 8 + iter_y])
					AddNode(list_begin, x, y, iter_x, iter_y);
				else
					break;
			}

			for (char iter_x = x - 1, iter_y = y + 1; iter_x > -1 && iter_y < 8; iter_x--, iter_y++) {
				if (!desk[iter_x * 8 + iter_y])
					AddNode(list_begin, x, y, iter_x, iter_y);
				else
					break;
			}

			for (char iter_x = x + 1, iter_y = y - 1; iter_x < 8 && iter_y > -1; iter_x++, iter_y--) {
				if (!desk[iter_x * 8 + iter_y])
					AddNode(list_begin, x, y, iter_x, iter_y);
				else
					break;
			}

			for (char iter_x = x - 1, iter_y = y - 1; iter_x > -1 && iter_y > -1; iter_x--, iter_y--) {
				if (!desk[iter_x * 8 + iter_y])
					AddNode(list_begin, x, y, iter_x, iter_y);
				else
					break;
			}
		}
	}
}

void SelectMove(moves_list_node* list_begin, Coordinates& cell_from, Coordinates& cell_to) {
	long long max_weight = -99999999999;
	moves_list_node* best_move = nullptr;
	int count_of_best_moves = 0;

	while (list_begin != nullptr) {
		if (list_begin->weight > max_weight) { //The evaluation function may assign a value smaller than the original one to max_weight, then nothing will be assigned to best_move
			DeleteList(best_move);
			count_of_best_moves = 1;

			max_weight = list_begin->weight;

			moves_list_node* new_move = new moves_list_node;
			new_move->cell_from = list_begin->cell_from;
			new_move->cell_to = list_begin->cell_to;
			new_move->p_next = nullptr;

			best_move = new_move;
		}
		else {
			if (list_begin->weight == max_weight) {
				++count_of_best_moves;

				moves_list_node* new_move = new moves_list_node;
				new_move->cell_from = list_begin->cell_from;
				new_move->cell_to = list_begin->cell_to;
				new_move->p_next = best_move;

				best_move = new_move;
			}
		}
		list_begin = list_begin->p_next;
	}

	moves_list_node* tmp = best_move;

	for (int i = randint(0, count_of_best_moves - 1); i > 0; i--)
		tmp = tmp->p_next;

	cell_from = tmp->cell_from;
	cell_to = tmp->cell_to;

	DeleteList(best_move);
}

void Desk::DoMove(char depth, char side_to_move, char bots_side, char avaliable_move, long long alpha, long long beta) {
	if (avaliable_move == ORDINARY_MOVE) {
		TryToOrdinaryMove(side_to_move);
	}
	else { // (*avaliable_move == FIGHTING_MOVE)
		chain_fight = 1;
		TryToFightingMove(side_to_move);
		while (1) {
			moves_list_node* list_begin_local = nullptr;

			AddMovesForChecker(&list_begin_local, cell_to.x, cell_to.y, side_to_move, FIGHTING_MOVE);

			if (list_begin_local == nullptr) {
				break;
			}
			EstimateMoves(list_begin_local, depth - 1, side_to_move, side_to_move, FIGHTING_MOVE, alpha, beta);
			SelectMove(list_begin_local, cell_from, cell_to);

			TryToFightingMove(side_to_move);
			DeleteList(list_begin_local);
		}
		chain_fight = 0;
	}
}

long long Desk::EstimateMoves(moves_list_node* list_begin, char depth, char side_to_move, char bots_side, char avaliable_move, long long alpha, long long beta) {
	long long global_weight = 1234567890;
	while (list_begin) {
		long long current_weight = 0;

		long long weight_for_backup = weight_for_first;
		char desk_for_backup[64];
		memcpy(desk_for_backup, desk, sizeof(char) << 6);

		cell_from = list_begin->cell_from;
		cell_to = list_begin->cell_to;

		DoMove(depth - 1, side_to_move, bots_side, avaliable_move, alpha, beta);

		char new_side = (FIRST_CHECKER + SECOND_CHECKER) - side_to_move;

		if (depth > 0) {

			moves_list_node* list_for_change_begin = nullptr;

			GenerateMoves(&list_for_change_begin, AvaliableMovesForAllCheckers(new_side), new_side);

			if (list_for_change_begin != nullptr) {
				current_weight = EstimateMoves(list_for_change_begin, depth - 1, new_side, bots_side, AvaliableMovesForAllCheckers(new_side), alpha, beta);
			}
			else {
				if (bots_side == side_to_move)
					current_weight = 99999999990; //Win
				else
					current_weight = -99999999990; //Lose
			}

			DeleteList(list_for_change_begin);
		}
		else {
			if (bots_side == FIRST_CHECKER)
				current_weight = weight_for_first;
			else
				current_weight = -weight_for_first;
		}

		list_begin->weight = current_weight;
		list_begin = list_begin->p_next;

		if (global_weight == 1234567890)
			global_weight = current_weight;

		if (side_to_move == bots_side) {
			if (current_weight > global_weight)
				global_weight = current_weight;

			if (global_weight > beta) {
				weight_for_first = weight_for_backup;
				memcpy(desk, desk_for_backup, sizeof(char) << 6);
				return global_weight;
			}

			if (global_weight > alpha)
				alpha = global_weight;
		}
		else {
			if (current_weight < global_weight)
				global_weight = current_weight;

			if (alpha > global_weight) {
				weight_for_first = weight_for_backup;
				memcpy(desk, desk_for_backup, sizeof(char) << 6);
				return global_weight;
			}
			if (global_weight < beta)
				beta = global_weight;
		}
		weight_for_first = weight_for_backup;
		memcpy(desk, desk_for_backup, sizeof(char) << 6);
	}
	return global_weight;
}

void Desk::BotsMove(char side_to_move, char bots_side) {
	avaliable_move = AvaliableMovesForAllCheckers(current_move); //Available moves
	moves_list_node* list_begin = nullptr;
	GenerateMoves(&list_begin, avaliable_move, bots_side);

	long long alpha = -99999999995;
	long long beta = +99999999995;

	EstimateMoves(list_begin, this->depth, side_to_move, bots_side, avaliable_move, alpha, beta);

	SelectMove(list_begin, cell_from, cell_to);

	DoMove(this->depth, side_to_move, bots_side, avaliable_move, alpha, beta);

	DeleteList(list_begin);

	AddStateToList(); //Add board state to list to track triple repetition

	current_move = (FIRST_CHECKER + SECOND_CHECKER) - current_move; //Move to another player
	avaliable_move = AvaliableMovesForAllCheckers(current_move); //Available moves for another player

	cell_from.x = -1; cell_from.y = -1;
	cell_to.x = -1;
	cell_to.y = -1;
}

char Desk::NoCheckers() { //Returns the winner
	char first = 0, second = 0;

	char* desk_ptr = &desk[0];
	while (desk_ptr++ != &desk[63]) {
		if (*desk_ptr % 10 == FIRST_CHECKER) first++;
		if (*desk_ptr % 10 == SECOND_CHECKER) second++;
	}

	if (!first) return SECOND_CHECKER;
	if (!second) return FIRST_CHECKER;

	return NO_ONE;
}

void Desk::HumansMove() {
	avaliable_move = AvaliableMovesForAllCheckers(current_move);
	if (avaliable_move == ORDINARY_MOVE) {
		CoordinatesToIndexes(cell_from, cell_to);

		if (TryToOrdinaryMove(current_move)) {
			current_move = (FIRST_CHECKER + SECOND_CHECKER) - current_move;

			AddStateToList(); //Add board state to list to track triple repetition

			cell_from.x = -1;
			cell_from.y = -1;
			cell_to.x = -1;
			cell_to.y = -1;
			avaliable_move = AvaliableMovesForAllCheckers(current_move);
		}
	}
	else {
		if (avaliable_move == FIGHTING_MOVE) {
			CoordinatesToIndexes(cell_from, cell_to);

			if (TryToFightingMove(current_move)) {
				if (AvaliableMovesForOneCheckers(cell_to.x, cell_to.y, current_move) == FIGHTING_MOVE) {
					chain_fight = 1;
					cell_from = cell_to;
				}
				else {
					chain_fight = 0;
					current_move = (FIRST_CHECKER + SECOND_CHECKER) - current_move;

					cell_from.x = -1;
					cell_from.y = -1;
					cell_to.x = -1;
					cell_to.y = -1;
					avaliable_move = AvaliableMovesForAllCheckers(current_move);
				}
			}
		}
	}
}

void CoordinatesToIndexes(Coordinates& coords_from, Coordinates& coords_to) {
	cout << "\rYour move in format(A1:B2):" << endl
		<< ": ";

	string diff = "";
	do {
		cout << "\r: ";
		cin >> diff;
	} while ('A' > diff[0] || 'H' < diff[0] ||
		'1' > diff[1] || '8' < diff[1] ||
		'A' > diff[3] || 'H' < diff[3] ||
		'1' > diff[4] || '8' < diff[4]);


	coords_from.y = diff[0] - 'A';
	coords_from.x = 7 - (char)(diff[1] - '1');

	coords_to.y = diff[3] - 'A';
	coords_to.x = 7 - (char)(diff[4] - '1');
}

char Desk::TryToFightingMove(char side) {
	if (!chain_fight && (cell_to.x == -1 || desk[cell_to.x * 8 + cell_to.y] % 10 == (FIRST_CHECKER + SECOND_CHECKER) - side)) { //If the destination cell is outside the field or there is a checker in it, then the selected checker is discarded
		cell_from.x = -1, cell_from.y = -1, cell_to.x = -1, cell_to.y = -1;
		return 0;
	}

	if (!chain_fight && desk[cell_to.x * 8 + cell_to.y] % 10 == side && AvaliableMovesForOneCheckers(cell_to.x, cell_to.y, side) == FIGHTING_MOVE) {
		cell_from.x = cell_to.x;
		cell_from.y = cell_to.y;
		return 0;
	}

	if (desk[cell_from.x * 8 + cell_from.y] == FIRST_CHECKER || desk[cell_from.x * 8 + cell_from.y] == SECOND_CHECKER) {
		if (abs(cell_from.x - cell_to.x) == 2 && abs(cell_from.y - cell_to.y) == 2 && desk[(cell_from.x + cell_to.x) / 2 * 8 + (cell_from.y + cell_to.y) / 2] % 10 == (FIRST_CHECKER + SECOND_CHECKER) - side) {
			if (side == FIRST_CHECKER) {
				if (desk[(cell_from.x + cell_to.x) / 2 * 8 + (cell_from.y + cell_to.y) / 2] == SECOND_CHECKER_D)
					weight_for_first += 3;
				else
					weight_for_first++;
			}
			else {
				if (desk[(cell_from.x + cell_to.x) / 2 * 8 + (cell_from.y + cell_to.y) / 2] == FIRST_CHECKER_D)
					weight_for_first -= 3;
				else
					weight_for_first--;
			}

			desk[(cell_from.x + cell_to.x) / 2 * 8 + (cell_from.y + cell_to.y) / 2] = 0;
			desk[cell_to.x * 8 + cell_to.y] = side;
			desk[cell_from.x * 8 + cell_from.y] = 0;

			if (desk[cell_to.x * 8 + cell_to.y] == FIRST_CHECKER && cell_to.x == 0) {
				desk[cell_to.x * 8 + cell_to.y] = FIRST_CHECKER_D;
				weight_for_first += 2;
			}


			if (desk[cell_to.x * 8 + cell_to.y] == SECOND_CHECKER && cell_to.x == 7) {
				desk[cell_to.x * 8 + cell_to.y] = SECOND_CHECKER_D;
				weight_for_first -= 2;
			}
			return 1;
		}
	}
	if (desk[cell_from.x * 8 + cell_from.y] == FIRST_CHECKER_D || desk[cell_from.x * 8 + cell_from.y] == SECOND_CHECKER_D) {
		if (abs(cell_from.x - cell_to.x) == abs(cell_from.y - cell_to.y) && (cell_from.y - cell_to.y)) {
			char checker_for_remove_x = cell_from.x, checker_for_remove_y = cell_from.y;
			char count_checkers = 0;

			char inc_x = (cell_from.x - cell_to.x > 0) ? 1 : -1;
			char inc_y = (cell_from.y - cell_to.y > 0) ? 1 : -1;

			for (char x = cell_from.x - inc_x, y = cell_from.y - inc_y; x != cell_to.x && y != cell_to.y; x -= inc_x, y -= inc_y) {
				if (desk[x * 8 + y] % 10 == side) count_checkers = -1000;
				if (desk[x * 8 + y] % 10 == (FIRST_CHECKER + SECOND_CHECKER) - side) {
					checker_for_remove_x = x;
					checker_for_remove_y = y;
					count_checkers++;
				}
			}

			if (count_checkers == 1) {
				if (side == FIRST_CHECKER) {
					if (desk[checker_for_remove_x * 8 + checker_for_remove_y] == SECOND_CHECKER_D)
						weight_for_first += 3;
					else
						weight_for_first++;
				}
				else {
					if (desk[checker_for_remove_x * 8 + checker_for_remove_y] == FIRST_CHECKER_D)
						weight_for_first -= 3;
					else
						weight_for_first--;
				}

				desk[checker_for_remove_x * 8 + checker_for_remove_y] = 0;

				desk[cell_to.x * 8 + cell_to.y] = desk[cell_from.x * 8 + cell_from.y];
				desk[cell_from.x * 8 + cell_from.y] = 0;


				return 1;
			}

		}
	}

	return 0;
}

char Desk::TryToOrdinaryMove(char side) {
	if (cell_to.x == -1 || desk[cell_to.x * 8 + cell_to.y] % 10 == (FIRST_CHECKER + SECOND_CHECKER) - side) { //If the destination cell is outside the field or there is an enemy checker in it, then the selected checker is discarded
		cell_from.x = -1, cell_from.y = -1, cell_to.x = -1, cell_to.y = -1;
		return 0;
	}

	if (desk[cell_to.x * 8 + cell_to.y] % 10 == side) {
		if (AvaliableMovesForOneCheckers(cell_to.x, cell_to.y, side) == ORDINARY_MOVE) {
			cell_from.x = cell_to.x;
			cell_from.y = cell_to.y;
		}
		else
			cell_from.x = -1, cell_from.y = -1, cell_to.x = -1, cell_to.y = -1;

		return 0;
	}

	if (desk[cell_from.x * 8 + cell_from.y] == FIRST_CHECKER || desk[cell_from.x * 8 + cell_from.y] == SECOND_CHECKER) {
		char diff = (desk[cell_from.x * 8 + cell_from.y] == FIRST_CHECKER) ? -1 : 1;

		if (cell_to.x - cell_from.x == diff && (abs(cell_to.y - cell_from.y) == 1)) { //Move one square diagonally

			desk[cell_to.x * 8 + cell_to.y] = side;
			desk[cell_from.x * 8 + cell_from.y] = 0;

			if (desk[cell_to.x * 8 + cell_to.y] == FIRST_CHECKER && cell_to.x == 0) {
				desk[cell_to.x * 8 + cell_to.y] = FIRST_CHECKER_D;
				weight_for_first += 2;
			}

			if (desk[cell_to.x * 8 + cell_to.y] == SECOND_CHECKER && cell_to.x == 7) {
				desk[cell_to.x * 8 + cell_to.y] = SECOND_CHECKER_D;
				weight_for_first -= 2;
			}

			return 1;
		}
	}

	if (desk[cell_from.x * 8 + cell_from.y] == FIRST_CHECKER_D || desk[cell_from.x * 8 + cell_from.y] == SECOND_CHECKER_D) {
		if (abs(cell_from.x - cell_to.x) == abs(cell_from.y - cell_to.y)) {
			char count_checkers = 0;

			char inc_x = (cell_from.x - cell_to.x > 0) ? 1 : -1;
			char inc_y = (cell_from.y - cell_to.y > 0) ? 1 : -1;

			for (char x = cell_from.x - inc_x, y = cell_from.y - inc_y; x != cell_to.x && y != cell_to.y; x -= inc_x, y -= inc_y)
				if (desk[x * 8 + y] % 10 == side) count_checkers = -1000;

			if (!count_checkers) {
				desk[cell_to.x * 8 + cell_to.y] = desk[cell_from.x * 8 + cell_from.y];
				desk[cell_from.x * 8 + cell_from.y] = 0;

				return 1;
			}
		}
	}

	return 0;
}

char Desk::AvaliableMovesForAllCheckers(char side) {
	char ordinary_moves = 0, fighting_moves = 0;

	for (char x = 0; x < 8; x++) {
		for (char y = x & 1 ^ 1; y < 8; y += 2) {
			if (desk[x * 8 + y] % 10 == side) {
				char avaliable_moves = AvaliableMovesForOneCheckers(x, y, side);
				if (avaliable_moves == FIGHTING_MOVE) fighting_moves++;
				if (avaliable_moves == ORDINARY_MOVE) ordinary_moves++;
			}
		}
	}

	if (!fighting_moves && !ordinary_moves) return NO_MOVES;
	if (fighting_moves) return FIGHTING_MOVE;
	else return ORDINARY_MOVE;
}

char Desk::AvaliableMovesForOneCheckers(char x, char y, char side) {
	char ordinary_moves = 0, fighting_moves = 0;

	char opposite_side = (FIRST_CHECKER + SECOND_CHECKER) - side;

	if (desk[x * 8 + y] == FIRST_CHECKER || desk[x * 8 + y] == SECOND_CHECKER) {
		char diff = (desk[x * 8 + y] == FIRST_CHECKER) ? -1 : 1;

		if (x + diff > -1 && x + diff < 8 && y + 1 < 8 && !desk[(x + diff) * 8 + y + 1]) ordinary_moves++;
		if (x + diff > -1 && x + diff < 8 && y - 1 > -1 && !desk[(x + diff) * 8 + y - 1]) ordinary_moves++;
		if ((unsigned char)(x + (diff << 1)) < 8 && y + 2 < 8 && !desk[(x + (diff << 1)) * 8 + y + 2] && desk[(x + diff) * 8 + y + 1] % 10 == opposite_side)
			fighting_moves++;
		if ((unsigned char)(x + (diff << 1)) < 8 && y - 2 > -1 && !desk[(x + (diff << 1)) * 8 + y - 2] && desk[(x + diff) * 8 + y - 1] % 10 == opposite_side)
			fighting_moves++;
		if ((unsigned char)(x - (diff << 1)) < 8 && y + 2 < 8 && !desk[(x - (diff << 1)) * 8 + y + 2] && desk[(x - diff) * 8 + y + 1] % 10 == opposite_side)
			fighting_moves++;
		if ((unsigned char)(x - (diff << 1)) < 8 && y - 2 > -1 && !desk[(x - (diff << 1)) * 8 + y - 2] && desk[(x - diff) * 8 + y - 1] % 10 == opposite_side)
			fighting_moves++;
	}
	else {
		char opponents_checkers = 0;

		for (char iter_x = x + 1, iter_y = y + 1; iter_x < 8 && iter_y < 8; iter_x++, iter_y++) {
			if (desk[iter_x * 8 + iter_y] % 10 == side) break;
			if (desk[iter_x * 8 + iter_y] % 10 == opposite_side) opponents_checkers++;
			if (opponents_checkers > 1) break;
			if (!desk[iter_x * 8 + iter_y]) {
				if (opponents_checkers) fighting_moves++;
				else ordinary_moves++;
			}
		}

		opponents_checkers = 0;
		for (char iter_x = x - 1, iter_y = y + 1; iter_x > -1 && iter_y < 8; iter_x--, iter_y++) {
			if (desk[iter_x * 8 + iter_y] % 10 == side) break;
			if (desk[iter_x * 8 + iter_y] % 10 == opposite_side) opponents_checkers++;
			if (opponents_checkers > 1) break;
			if (!desk[iter_x * 8 + iter_y]) {
				if (opponents_checkers) fighting_moves++;
				else ordinary_moves++;
			}
		}

		opponents_checkers = 0;
		for (char iter_x = x + 1, iter_y = y - 1; iter_x < 8 && iter_y > -1; iter_x++, iter_y--) {
			if (desk[iter_x * 8 + iter_y] % 10 == side) break;
			if (desk[iter_x * 8 + iter_y] % 10 == opposite_side) opponents_checkers++;
			if (opponents_checkers > 1) break;
			if (!desk[iter_x * 8 + iter_y]) {
				if (opponents_checkers) fighting_moves++;
				else ordinary_moves++;
			}
		}

		opponents_checkers = 0;
		for (char iter_x = x - 1, iter_y = y - 1; iter_x > -1 && iter_y > -1; iter_x--, iter_y--) {
			if (desk[iter_x * 8 + iter_y] % 10 == side) break;
			if (desk[iter_x * 8 + iter_y] % 10 == opposite_side) opponents_checkers++;
			if (opponents_checkers > 1) break;
			if (!desk[iter_x * 8 + iter_y]) {
				if (opponents_checkers) fighting_moves++;
				else ordinary_moves++;
			}
		}
	}

	if (!fighting_moves && !ordinary_moves) return NO_MOVES;
	if (fighting_moves) return FIGHTING_MOVE;
	else return ORDINARY_MOVE;
}

void Desk::SetDefault() {
	weight_for_first = 0;

	memset(desk, 0x00, sizeof(char) << 6);

	for (char i = 0; i < 3; i++)
		for (char j = (i + 1) & 1; j < 8; j += 2) {
			desk[i * 8 + j] = SECOND_CHECKER;
		}

	for (char i = 5; i < 8; i++)
		for (char j = (i + 1) & 1; j < 8; j += 2) {
			desk[i * 8 + j] = FIRST_CHECKER;
		}

	ClearDeskStatesList();
}

void Desk::SetCurrentMove(char side) {
	current_move = side;
}

char Desk::GetCurrentMove() {
	return current_move;
}

void Desk::SetDepth(char depth) {
	this->depth = depth;
}

char Desk::GetResult() {
	return result;
}

void Desk::SetSideSelected(char side) {
	side_selected = side;
}

char Desk::GetSideSelected() {
	return side_selected;
}

void Desk::SetChecker(Coordinates& coords, char checker) {
	if ((checker == FIRST_CHECKER && coords.x == 0) || (checker == SECOND_CHECKER && coords.x == 7))
		checker *= 11;
	if ((coords.x + coords.y) & 1 != 0)
		desk[coords.x * 8 + coords.y] = checker;
}

void Desk::SetAvaliableMove() {
	avaliable_move = AvaliableMovesForAllCheckers(current_move);
}

void Desk::ClearDeskStatesList() {
	while (desk_states_list != nullptr) {
		desk_state* tmp = desk_states_list;
		desk_states_list = desk_states_list->p_next;
		delete tmp;
	}

	desk_states_list = nullptr;

	max_repetitation = 1;
}

void Desk::AddStateToList() {
	if (desk_states_list == nullptr) {
		desk_state* new_state = new desk_state;
		memcpy(new_state->desk, desk, sizeof(char) << 6);
		new_state->count = 1;
		new_state->p_next = nullptr;
		desk_states_list = new_state;

		return;
	}

	desk_state* iter = desk_states_list;
	while (iter->p_next != nullptr) {
		if (!memcmp(desk, iter->desk, sizeof(char) << 6)) {
			iter->count++;
			if (iter->count > max_repetitation)
				max_repetitation = iter->count;
			return;
		}
		iter = iter->p_next;
	}

	desk_state* new_state = new desk_state;
	memcpy(new_state->desk, desk, sizeof(char) << 6);
	new_state->count = 1;
	new_state->p_next = nullptr;
	iter->p_next = new_state;
}

Desk::~Desk() {
	ClearDeskStatesList();
}