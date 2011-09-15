/**
    This file is part of Sudoku United.

    Sudoku United is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Sudoku United is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Sudoku United.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SUDOKU_H
#define SUDOKU_H

#include <QObject>
#include <QtDeclarative>

#include "boardgenerator.h"

class MultiplayerAdapter;
class Player;
class Game;
class GameInfo;
class GameInfoModel;

class Sudoku : public QObject
{
    Q_OBJECT
    // Q_PROPERTY(MultiplayerAdapter * multiplayerAdapter READ multiplayerAdapter CONSTANT)
    Q_PROPERTY(Player * player READ player CONSTANT)
    Q_PROPERTY(Game * game READ game NOTIFY gameChanged)
public:
    explicit Sudoku(QObject *parent = 0);

    Player *player() const { return m_player; }

    Game *game() const { return m_game; }

    Q_INVOKABLE
    virtual void join(GameInfo *game);

    Q_INVOKABLE
    virtual GameInfoModel *discoverGames();

    Q_INVOKABLE
    virtual Game *createGame(BoardGenerator::Difficulty difficulty = BoardGenerator::EASY);

    Q_INVOKABLE
    virtual void leave();

    static Sudoku *instance();
signals:
    void gameChanged();
public slots:

protected slots:
    void setGame(Game *game);
private:
    void addMultiplayerAdapter(MultiplayerAdapter *adapter);

private:
    QList<MultiplayerAdapter *> m_multiplayerAdapters;
    Player *m_player;
    Game *m_game;
private:
    static Sudoku *m_instance;

};

QML_DECLARE_TYPE(Sudoku)

#endif // SUDOKU_H
