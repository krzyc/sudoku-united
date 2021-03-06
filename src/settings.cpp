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

#include "settings.h"
#include "game.h"
#include "highscore.h"

Settings::Settings(QObject *parent) :
    QSettings("Bithub", "Sudoku United", parent),
    m_hapticFeedbackEnabled(true),
    m_bluetoothEnabled(true),
    m_lastGame(NULL),
    m_showGameTimer(false),
    m_highscoreModel(NULL),
    m_quickAccessAction(UndoAction),
    m_showedJoinHelp(false),
    m_longPressAction(InsertNote)
{
    loadSettings();
}

void Settings::loadSettings() {
    setPlayerName(value("playerName", "Player").toString());
    setHapticFeedbackEnabled(value("hapticFeedbackEnabled", true).toBool());
    setBluetoothEnabled(value("bluetoothEnabled", true).toBool());
#ifdef Q_OS_BLACKBERRY
    setShowGameTimer(value("showGameTimer", true).toBool());
#else
    setShowGameTimer(value("showGameTimer", false).toBool());
#endif
    setQuickAccessAction((QuickAccessAction) value("quickAccessAction", 0).toUInt());
    setShowedJoinHelp(value("showedJoinHelp", false).toBool());
    setLongPressAction((LongPressAction) value("longPressAction", Settings::InsertNote).toUInt());

    m_playerUuid = value("playerUuid", QVariant::fromValue(QUuid::createUuid())).value<QUuid>();

    QVariant lastGameVariant = value("lastGame");
    if (!lastGameVariant.isNull()) {
        QDataStream gameStream(lastGameVariant.toByteArray());

        m_lastGame = new Game(this);
        gameStream >> *m_lastGame;

        if (gameStream.status() != QDataStream::Ok) {
            delete m_lastGame;
            m_lastGame = NULL;
        }
    } else {
        m_lastGame = NULL;
    }

    QVariant highscoreVariant = value("highscore");
    if (!highscoreVariant.isNull()){
        QDataStream highscoreStream(highscoreVariant.toByteArray());

        m_highscoreModel = new HighscoreModel(this);
        highscoreStream >> *m_highscoreModel;

        if (highscoreStream.status() != QDataStream::Ok) {
            delete m_highscoreModel;
            m_highscoreModel = NULL;
        }
    }
    if (m_highscoreModel == NULL) {
        m_highscoreModel = new HighscoreModel(this);
    }
}

void Settings::saveSettings() {
    setValue("playerName", playerName());
    setValue("hapticFeedbackEnabled", hapticFeedbackEnabled());
    setValue("bluetoothEnabled", bluetoothEnabled());
    setValue("showGameTimer", showGameTimer());
    setValue("quickAccessAction", (quint32) quickAccessAction());
    setValue("playerUuid", QVariant::fromValue(playerUuid()));
    setValue("showedJoinHelp", showedJoinHelp());
    setValue("longPressAction", (quint32) longPressAction());

    if (m_lastGame) {
        QByteArray buffer;
        QDataStream gameStream(&buffer, QIODevice::WriteOnly);
        gameStream << *m_lastGame;

        setValue("lastGame", buffer);
    } else {
        remove("lastGame");
    }
    if (m_highscoreModel) {
        QByteArray highscoreBuffer;
        QDataStream highscoreStream(&highscoreBuffer, QIODevice::WriteOnly);
        highscoreStream << *m_highscoreModel;

        setValue("highscore", highscoreBuffer);
    } else {
        remove("highscore");
    }
    sync();
}

QString Settings::playerName() const {
    return m_playerName;
}

void Settings::setPlayerName(const QString &playerName) {
    if (playerName == m_playerName)
        return;

    m_playerName = playerName;

    emit playerNameChanged();
}

bool Settings::hapticFeedbackEnabled() const {
    return m_hapticFeedbackEnabled;
}

void Settings::setHapticFeedbackEnabled(bool enableHapticFeedback) {
    if (m_hapticFeedbackEnabled == enableHapticFeedback)
        return;

    m_hapticFeedbackEnabled = enableHapticFeedback;

    emit hapticFeedbackEnabled();
}

bool Settings::bluetoothEnabled() const {
    return m_bluetoothEnabled;
}

void Settings::setBluetoothEnabled(bool enableBluetooth) {
    if (m_bluetoothEnabled == enableBluetooth)
        return;

    m_bluetoothEnabled = enableBluetooth;

    emit bluetoothEnabledChanged();
}

Game *Settings::lastGame() const {
    return m_lastGame;
}

void Settings::setLastGame(Game *game) {
    if (m_lastGame && m_lastGame->parent() == this)
        m_lastGame->deleteLater();

    if (game) {
        m_lastGame = game;
        m_lastGame->setParent(this);
    } else {
        m_lastGame = NULL;
    }
}

bool Settings::showGameTimer() const {
    return m_showGameTimer;
}

void Settings::setShowGameTimer(bool shown) {
    if (shown == m_showGameTimer)
        return;

    m_showGameTimer = shown;

    emit showGameTimerChanged();
}

HighscoreModel *Settings::highscoreModel() const
{
    return m_highscoreModel;
}

void Settings::setHighscoreModel(HighscoreModel *highscoreModel)
{
    m_highscoreModel = highscoreModel;
}


Settings::QuickAccessAction Settings::quickAccessAction() const
{
    return m_quickAccessAction;
}

void Settings::setQuickAccessAction(Settings::QuickAccessAction action)
{
    if (m_quickAccessAction == action)
        return;

    m_quickAccessAction = action;
    emit quickAccessActionChanged();
}

QUuid Settings::playerUuid() const {
    return m_playerUuid;
}

bool Settings::showedJoinHelp() const {
    return m_showedJoinHelp;
}

void Settings::setShowedJoinHelp(bool showed) {
    if (showed == m_showedJoinHelp)
        return;

    m_showedJoinHelp = showed;

    emit showedJoinHelpChanged();
}

Settings::LongPressAction Settings::longPressAction() const
{
    return m_longPressAction;
}

void Settings::setLongPressAction(Settings::LongPressAction action)
{
    if (m_longPressAction == action)
        return;

    m_longPressAction = action;

    emit longPressActionChanged();
}
