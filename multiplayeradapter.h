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

#ifndef MULTIPLAYERADAPTER_H
#define MULTIPLAYERADAPTER_H

#include <QObject>
#include <QtDeclarative>
#include "boardgenerator.h"

class Player;
class GameInfo;
class Game;

class Cell;

class Message;
class GameMessage;
class SetValueMessage;
class JoinMessage;
class HelloMessage;

class GameInfoModel;

class PlayerInfo {
public:
    enum State {
        Connecting = 1,
        Connected = 2,
        Disconnected = 4
    };
public:
    PlayerInfo();
public:
    QIODevice *device;
    QByteArray buffer;
    State state;
    Player *player;
};

class GameInfo : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
public:
    GameInfo(QObject *parent = 0);

    virtual QString name() const = 0;
    virtual quint16 playerCount() const = 0;
};

QML_DECLARE_TYPE(GameInfo)

class MultiplayerAdapter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Game * game READ game NOTIFY gameChanged)

public:
    const static quint16 ProtocolVersion = 1;

    enum PlayerStateFilter {
        Connecting = 1,
        Connected = 2,
        Online = Connecting | Connected
    };

public:
    explicit MultiplayerAdapter(QObject *parent);

    Q_INVOKABLE
    virtual void join(GameInfo *game) = 0;
    Q_INVOKABLE
    virtual GameInfoModel *discoverGames() = 0;
    Q_INVOKABLE
    virtual Game *createGame(BoardGenerator::Difficulty difficulty = BoardGenerator::EASY);
    Q_INVOKABLE
    virtual void leave();

    Game *game() const { return m_game; }
protected slots:
    virtual void onCellValueChanged(Cell *cell);
    virtual void onConnectedToServer();
protected:
    virtual void setGame(Game *game);
    virtual void sendMessage(const PlayerInfo *info, Message *message, PlayerStateFilter stateFilter=Online);
    virtual void handleNewConnection(QIODevice *device);
protected:
    PlayerInfo *m_local;
    QHash<QIODevice *, PlayerInfo *> m_remote;
private slots:
    void onBoardChanged();
    void onReadyRead();
    void onReadChannelFinished();
    void onSocketDestroyed(QObject *socket);
private:
    void parseMessages(PlayerInfo &playerInfo);

    void handleHelloMessage(PlayerInfo &playerInfo, HelloMessage *message);
    void handleJoinMessage(PlayerInfo &playerInfo, JoinMessage *message);
    void handleGameMessage(PlayerInfo &playerInfo, GameMessage *message);
    void handleSetValueMessage(PlayerInfo &playerInfo, SetValueMessage *message);
signals:
    /**
      * Indicates that a game has been set on this multiplayer adapter. This usually means that a game has started.
      */
    void gameChanged();

    /**
      * Indicates that joining a game has failed.
      */
    void joinFailed(const QString &reason);

    /**
      * Indicates that a new player has joined the game.
      */
    void playerJoined(Player *player);

    /**
      * Indicates that a player has disconnected from the game.
      */
    void playerLeft(Player *player);

    /**
      * Indicates that the game was left (e.g. due to disconnection from the server or due to call to leave()).
      */
    void gameLeft();
private:
    Game *m_game;
};

QML_DECLARE_TYPE(MultiplayerAdapter)

class GameInfoModel : public QAbstractListModel {
    Q_OBJECT
    Q_ENUMS(State)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
public:
    enum State {
        Discovering,
        Complete
    };

    State state() const { return m_state; }

    virtual QVariant data(const QModelIndex &index, int role) const;

    int rowCount(const QModelIndex &parent) const;
signals:
    void stateChanged();
protected:
    GameInfoModel(QObject *parent = 0);

    void appendGameInfo(GameInfo *gameInfo);
protected:
    QList<GameInfo *> m_gameInfoList;
    State m_state;
private:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PlayerCountRole,
        GameInfoRole
    };
};

QML_DECLARE_TYPE(GameInfoModel)
#endif // MULTIPLAYERADAPTER_H
