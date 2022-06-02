#include <iostream>
#include <string>

using namespace std;

enum {
	BEG,
	END
};
enum {
	NEXT,
	PREV
};


class element {
public:
	int wartosc;
	element* xorAdresow;
	element(int value) {
		wartosc = value;
		xorAdresow = nullptr;
	};
	element(int value, element* xorAdr) {
		xorAdresow = xorAdr;
		wartosc = value;
	};
};

class list {
public:
	element* first, * last;
	element* actual, * next, * prev;
	int size;

	list() {
		first = last = actual = prev = next = nullptr;
		size = 0;
	}
	~list();

	element* operacjaXOR(element* a, element* b) {
		return (element*)((size_t)a ^ (size_t)b);
	}

	void ustawPrevNext();
	void act();
	void move(int dir);
	void addBegEnd(int value, int where);
	void addAct(int value);
	void delBegEnd(element*& kraniecUsuwany, element*& kraniecModyfikowany);
	void delAct();
	void delVal(int value);
	void del(element* usuwany, element* sasiad1, element* sasiad2);
	void print(int from);
};


list::~list() {
	while (actual) delAct();
}

// metody klasy list:
//ustawienie wartosci previous i next
void list::ustawPrevNext() {
	if (!prev) prev = operacjaXOR(actual->xorAdresow, next);
	if (!next) next = operacjaXOR(actual->xorAdresow, prev);
};

//wyczytanie wartosci ACTUAL
void list::act() {
	if (actual)
		cout << actual->wartosc << endl;
	else cout << "NULL\n";
};

//wyczytanie wartosci nastepnego/poprzedniego elementu i przesuniecie ACTUAL
void list::move(int dir) {
	if (size == 0) {
		cout << "NULL\n";
		return;
	}
	ustawPrevNext();
	if (dir == NEXT) {
		prev = actual;
		actual = next;
		next = operacjaXOR(prev, actual->xorAdresow);
	}
	else {
		next = actual;
		actual = prev;
		prev = operacjaXOR(next, actual->xorAdresow);
	}
	cout << actual->wartosc << endl;
};

//dodawanie elementu przed aktualnym
void list::addAct(int value) {
	element* nowy;
	if (!actual) return;	//lista pusta
	else if (size == 1) {	//lista jednoelementowa
		nowy = new element(value, operacjaXOR(actual, actual));
		actual->xorAdresow = operacjaXOR(nowy, nowy);
		prev = next = first = nowy;
		return;
	}
	else {
		ustawPrevNext();
		nowy = new element(value, operacjaXOR(actual, prev));
		prev->xorAdresow = operacjaXOR(nowy, operacjaXOR(prev->xorAdresow, actual));
		actual->xorAdresow = operacjaXOR(nowy, next);
		prev = nowy;
		if (actual == first) first = nowy;
	}
	size++;
};

//dodawanie elementu na poczatek lub koniec listy
void list::addBegEnd(int value, int where) {
	element* nowy = new element(value);
	size++;
	if (size == 1) {
		first = last = actual = prev = next = nowy;
		nowy->xorAdresow = operacjaXOR(nowy, nowy);
		return;
	}
	else if (size == 2) {
		prev = next = nowy;
		if (where == BEG) first = nowy;
		else last = nowy;
		nowy->xorAdresow = operacjaXOR(actual, actual);
		return;
	}

	last->xorAdresow = operacjaXOR(nowy, operacjaXOR(first, last->xorAdresow));
	first->xorAdresow = operacjaXOR(nowy, operacjaXOR(last, first->xorAdresow));
	nowy->xorAdresow = operacjaXOR(first, last);

	if (actual == first) prev = nowy;
	else if (actual == last) next = nowy;

	if (where == BEG) {
		first = nowy;
	}
	else if (where == END) {
		last = nowy;
	}
};

//usuwanie elementu
void list::del(element* usuwany, element* sasiad1, element* sasiad2) {
	sasiad1->xorAdresow = operacjaXOR(sasiad2, operacjaXOR(usuwany, sasiad1->xorAdresow));
	sasiad2->xorAdresow = operacjaXOR(sasiad1, operacjaXOR(usuwany, sasiad2->xorAdresow));
	delete usuwany;
}

//usuwanie z poczatku lub konca listy
void list::delBegEnd(element*& kraniecUsuwany, element*& kraniecModyfikowany) {
	if (size == 0) return;
	else if (size == 2) {
		kraniecModyfikowany->xorAdresow = operacjaXOR(kraniecModyfikowany, kraniecModyfikowany);
		delete kraniecUsuwany;
		kraniecUsuwany = prev = next = actual = kraniecModyfikowany;
	}
	else if (size == 1) {
		delete kraniecUsuwany;
		kraniecUsuwany = kraniecModyfikowany = actual = prev = next = nullptr;
	}
	else {
		ustawPrevNext();
		if (actual == kraniecUsuwany) {
			prev = operacjaXOR(prev->xorAdresow, actual);
			actual = operacjaXOR(actual->xorAdresow, next);
		}
		else if (prev == kraniecUsuwany) {
			if (kraniecUsuwany == first) prev = last;
			else prev = operacjaXOR(last->xorAdresow, first);
		}
		else if (next == kraniecUsuwany) {
			if (kraniecUsuwany == last) next = first;
			else next = operacjaXOR(first->xorAdresow, last);
		}
		element* second = operacjaXOR(kraniecUsuwany->xorAdresow, kraniecModyfikowany);
		del(kraniecUsuwany, kraniecModyfikowany, second);
		kraniecUsuwany = second;
	}
	size--;
}

//usuwanie aktualnego elementu i aktualizacja wskaznika ACTUAL
void list::delAct() {
	if (actual == first) {
		delBegEnd(first, last);
		return;
	}
	else if (actual == last) {
		delBegEnd(last, first);
		return;
	}
	//tablica 0-, 1- i 2elementowa spełniają powyższe warunki i są obsługiwane w funkcji delBegEnd()
	
	ustawPrevNext();
	del(actual, prev, next);
	actual = prev;
	prev = operacjaXOR(next, actual->xorAdresow);
	size--;
}

//usuwanie elementow o zadanej wartosci
void list::delVal(int value) {
	while (actual && actual->wartosc == value) delAct();	// po tym bloku instrukcji elementy
	if (size == 0) return;									// actual i prev zostana zaktualizowane
	ustawPrevNext();										// i przechowane we wskaznikach tmp i tmPrev
	while (prev && prev->wartosc == value) {
		next = actual;
		actual = prev;
		prev = operacjaXOR(actual->xorAdresow, next);
		delAct();

		prev = actual;
		actual = next;
		next = operacjaXOR(prev, actual->xorAdresow);
	}

	if (!actual || actual == prev) return;
	element* tmp = actual;
	element* tmPrev = prev;
	int S = size;

	actual = first;
	next = operacjaXOR(last, first->xorAdresow);
	prev = last;

	for (int i = 0; i < S; i++) {
		if (actual->wartosc == value) delAct(); // wykorzystanie metody DEL_ACT do oczyszczenia listy
		prev = actual;
		actual = next;
		next = operacjaXOR(actual->xorAdresow, prev);
	}

	actual = tmp;
	prev = tmPrev;
	next = operacjaXOR(actual->xorAdresow, prev);
}

//drukowanie listy
void list::print(int from) {
	if (!actual) {
		cout << "NULL\n";
		return;
	}
	else if (size == 1) {
		cout << first->wartosc << endl;
		return;
	}
	element* curr, * currNext, * tmp;
	if (from == BEG) {
		curr = first;
		currNext = operacjaXOR(first->xorAdresow, last);
	}
	else {
		curr = last;
		currNext = operacjaXOR(last->xorAdresow, first);
	}

	cout << curr->wartosc << " ";
	for (int i = 1; i < size; i++) {
		tmp = curr;
		curr = currNext;
		currNext = operacjaXOR(curr->xorAdresow, tmp);
		cout << curr->wartosc << " ";
	}
	cout << endl;
}

int main() {
	string s;
	int n;
	list lista;
	while (cin >> s) {
		if (s == "ACTUAL") lista.act();
		else if (s == "NEXT") lista.move(NEXT);
		else if (s == "PREV") lista.move(PREV);
		else if (s == "ADD_BEG") {
			cin >> n;
			lista.addBegEnd(n, BEG);
		}
		else if (s == "ADD_END") {
			cin >> n;
			lista.addBegEnd(n, END);
		}
		else if (s == "ADD_ACT") {
			cin >> n;
			lista.addAct(n);
		}
		else if (s == "DEL_BEG") lista.delBegEnd(lista.first, lista.last);
		else if (s == "DEL_END") lista.delBegEnd(lista.last, lista.first);
		else if (s == "DEL_VAL") {
			cin >> n;
			lista.delVal(n);
		}
		else if (s == "DEL_ACT") lista.delAct();
		else if (s == "PRINT_FORWARD") lista.print(BEG);
		else if (s == "PRINT_BACKWARD") lista.print(END);
		//else if (s == "Q") break;
		//else cout << "np\n";
	}
	return 0;
}