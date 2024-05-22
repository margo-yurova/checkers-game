#include <QApplication> //управление жизненным циклом игры
#include <QWidget> //основа всех элементов интерфейса
#include <QPushButton>//кнопки на поле
#include <QGridLayout>//расположение кнопок в виде сетки
#include <QSignalMapper>//связывание кнопок и их событий
#include <QLabel>//отображение текста
#include <QHBoxLayout>//расположение кнопок в строке
#include <QVBoxLayout>//расположение кнопок в столбце
#include <QMessageBox>// диалоговые окна 

// Класс CheckersGame наследуется от QWidget и является основным классом для игры
class CheckersGame : public QWidget {
		Q_OBJECT

public:
		// Конструктор класса
		CheckersGame(QWidget *parent = nullptr) : QWidget(parent) {
				setWindowTitle("Checkers Game");

				// Создаем основной вертикальный компоновщик
				QVBoxLayout *mainLayout = new QVBoxLayout(this);

				// Создаем горизонтальный компоновщик для информации о ходе и счете
				QHBoxLayout *infoLayout = new QHBoxLayout();

				// Создаем сеточный компоновщик для доски
				QGridLayout *layout = new QGridLayout();
				const int boardSize = 8;

				// Создаем метки для отображения текущего хода и счета
				turnLabel = new QLabel("White's turn", this);
				whiteScoreLabel = new QLabel("White: 0", this);
				blackScoreLabel = new QLabel("Black: 0", this);

				// Добавляем метки в горизонтальный компоновщик
				infoLayout->addWidget(turnLabel);
				infoLayout->addWidget(whiteScoreLabel);
				infoLayout->addWidget(blackScoreLabel);

				// Создаем объект QSignalMapper для упрощения связи сигналов кнопок с обработчиком
				QSignalMapper *signalMapper = new QSignalMapper(this);

				// Инициализация кнопок доски
				for (int row = 0; row < boardSize; ++row) {
						for (int col = 0; col < boardSize; ++col) {
								QPushButton *button = new QPushButton(this);
								button->setFixedSize(45, 45);

								if ((row + col) % 2 == 1) {
										button->setStyleSheet("background-color: #D18B47;");
										if (row < 3) {
												button->setText("B");
												button->setStyleSheet("background-color: #D18B47; color: black; border-radius: 22px;");
										} else if (row > 4) {
												button->setText("W");
												button->setStyleSheet("background-color: #D18B47; color: white; border-radius: 22px;");
										}
										signalMapper->setMapping(button, QString("%1,%2").arg(row).arg(col));
										connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
								} else {
										button->setStyleSheet("background-color: #FFCE9E;");
								}

								board[row][col] = button;
								layout->addWidget(button, row, col);
						}
				}

				// Добавляем компоновщики в основной компоновщик
				mainLayout->addLayout(infoLayout);
				mainLayout->addLayout(layout);

				// Подключаем сигнал от маппера к слоту для обработки кликов по кнопкам
				connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(onCellClicked(QString)));
		}

private slots:
		// Слот для обработки кликов по кнопкам
		void onCellClicked(const QString &position) {
				QStringList coordinates = position.split(",");
				int row = coordinates.at(0).toInt();
				int col = coordinates.at(1).toInt();

				if (!selected) {
						if (board[row][col]->text().isEmpty() ||
								(board[row][col]->text() == "W" && !whiteTurn) ||
								(board[row][col]->text() == "B" && whiteTurn)) return;
						selectedRow = row;
						selectedCol = col;
						selected = true;
						board[row][col]->setStyleSheet(board[row][col]->styleSheet() + "; border: 2px solid red;");
				} else {
						if ((row + col) % 2 == 1 && isValidMove(selectedRow, selectedCol, row, col)) {
								board[row][col]->setText(board[selectedRow][selectedCol]->text());
								board[row][col]->setStyleSheet("background-color: #D18B47; color: " +
										QString(board[selectedRow][selectedCol]->text() == "W" ? "white" : "black") + "; border-radius: 22px;");
								board[selectedRow][selectedCol]->setText("");
								board[selectedRow][selectedCol]->setStyleSheet("background-color: #D18B47;");

								// Обработка захвата шашки
								if (abs(selectedRow - row) == 2 && abs(selectedCol - col) == 2) {
										int midRow = (selectedRow + row) / 2;
										int midCol = (selectedCol + col) / 2;
										board[midRow][midCol]->setText("");
										board[midRow][midCol]->setStyleSheet("background-color: #D18B47;");
										if (whiteTurn) {
												++blackScore;
										} else {
												++whiteScore;
										}
										updateScores();
								}
								selected = false;
								whiteTurn = !whiteTurn;
								updateTurnLabel();
								checkForWin();
						} else {
								board[selectedRow][selectedCol]->setStyleSheet(board[selectedRow][selectedCol]->styleSheet().remove("; border: 2px solid red;"));
								selected = false;
						}
				}
		}

private:
		// Объявление переменных класса
		QPushButton *board[8][8];  // Кнопки доски
		QLabel *turnLabel;         // Метка текущего хода
		QLabel *whiteScoreLabel;   // Метка счета белых
		QLabel *blackScoreLabel;   // Метка счета черных
		int selectedRow, selectedCol; // Координаты выбранной клетки
		bool selected = false;       // указание на выбор клетки
		bool whiteTurn = true;       // указание на текущий ход (true - белые, false - черные)
		int whiteScore = 0;          // Счет белых
		int blackScore = 0;          // Счет черных

		// Функция проверки возможности хода
		bool isValidMove(int fromRow, int fromCol, int toRow, int toCol) {
			if (!board[toRow][toCol]->text().isEmpty()) { return false;} // Если в целевой клетке уже есть шашка, ход недействителен 
				if (abs(fromRow - toRow) == 1 && abs(fromCol - toCol) == 1) {
						return board[toRow][toCol]->text().isEmpty();
				}
				if (abs(fromRow - toRow) == 2 && abs(fromCol - toCol) == 2) {
						int midRow = (fromRow + toRow) / 2;
						int midCol = (fromCol + toCol) / 2;
						if (!board[midRow][midCol]->text().isEmpty() && board[midRow][midCol]->text() != board[fromRow][fromCol]->text()) {
								return true;
						}
				}
				return false;
		}

		// Обновление метки текущего хода
		void updateTurnLabel() {
				turnLabel->setText(whiteTurn ? "White's turn" : "Black's turn");
		}

		// Обновление счетов
		void updateScores() {
				whiteScoreLabel->setText("White: " + QString::number(whiteScore));
				blackScoreLabel->setText("Black: " + QString::number(blackScore));
		}

		// Проверка на победу
		void checkForWin() {
				if (whiteScore == 12) {
						turnLabel->setText("Black wins!");
						endGame();
				} else if (blackScore == 12) {
						turnLabel->setText("White wins!");
						endGame();
				}
		}

		// Завершение игры
		void endGame() {
				// Отключаем все кнопки
				for (int row = 0; row < 8; ++row) {
						for (int col = 0; col < 8; ++col) {
								board[row][col]->setEnabled(false);
						}
				}
				// Показать сообщение с информацией о победе
				QMessageBox::information(this, "Game Over", turnLabel->text());
		}
};

int main(int argc, char *argv[]) {
		QApplication app(argc, argv);
		CheckersGame window;
		window.show();
		return app.exec();
}

#include "main.moc"