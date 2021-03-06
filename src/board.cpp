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

#include "board.h"
#include "player.h"
#include "boardgenerator.h"
#include "notemodel.h"

ModificationLogEntry::ModificationLogEntry() {

}

/**
  * Creates a new log entry storing the cell's current value and value owner.
  */
ModificationLogEntry::ModificationLogEntry(const Cell *cell) {
    m_x = cell->x();
    m_y = cell->y();
    m_value = cell->value();
    m_valueOwner = cell->valueOwner();
}

ModificationLogEntry::ModificationLogEntry(quint8 x,
                                          quint8 y,
                                          quint8 value,
                                          Player *valueOwner) :
    m_x(x), m_y(y), m_value(value), m_valueOwner(valueOwner) {

}

/**
  * \return The X coordinate of the modified cell.
  */
quint8 ModificationLogEntry::x() const {
    return m_x;
}

/**
  * \return the Y coordinate of the modified cell.
  */
quint8 ModificationLogEntry::y() const {
    return m_y;
}

/**
  * \return The value the cell had at the time at which the log entry was
  *         created.
  */
quint8 ModificationLogEntry::value() const {
    return m_value;
}

/**
  * \return The value owner the cell had at the time at which the log entry was
  *         created.
  */
Player *ModificationLogEntry::valueOwner() const {
    return m_valueOwner;
}

Board::Board(const Board &other, QObject *parent) :
    QObject(parent ? parent : other.parent()) {

    memcpy(m_cellValues, other.m_cellValues, sizeof(m_cellValues));

    for (quint8 x = 0; x < 9; x++) {
        for (quint8 y = 0; y < 9; y++) {
            m_cells[x][y] = other.m_cells[x][y];
            m_cells[x][y].setParent(this);
            m_solution[x][y] = other.m_solution[x][y];

            connect(&m_cells[x][y],
                    SIGNAL(valueChanged()),
                    SLOT(onCellValueChanged()));
            connect(&m_cells[x][y],
                    SIGNAL(beforeValueChanged()),
                    SLOT(onBeforeCellValueChanged()));
        }
    }

    m_startTime = other.m_startTime;
    m_elapsedTime = other.m_elapsedTime;
    m_paused = other.m_paused;
    m_difficulty = other.m_difficulty;
    if (other.m_selectedCell) {
        m_selectedCell = &(m_cells[other.m_selectedCell->x()][other.m_selectedCell->y()]);
    } else {
        m_selectedCell = NULL;
    }
    blockModificationLog = other.blockModificationLog;
}

Board::Board(QObject *parent) :
    QObject(parent), blockModificationLog(false), m_selectedCell(NULL)
{
    for (quint8 y = 0; y < 9; y++) {
        for (quint8 x = 0; x < 9; x++) {
            m_cellValues[x][y] = 0;
            m_cells[x][y].m_x = x;
            m_cells[x][y].m_y = y;
            m_cells[x][y].setParent(this);

            connect(&m_cells[x][y],
                    SIGNAL(valueChanged()),
                    SLOT(onCellValueChanged()));
            connect(&m_cells[x][y],
                    SIGNAL(beforeValueChanged()),
                    SLOT(onBeforeCellValueChanged()));
        }
    }

    m_startTime = QDateTime::currentMSecsSinceEpoch();
    m_elapsedTime = 0;
    m_difficulty = Sudoku::SIMPLE;
    m_paused = false;
}

Cell *Board::cellAt(quint8 x, quint8 y) const {
    if (x >= 9 || y >= 9) {
        qmlInfo(this) << "Invalid cell index specified.";

        return NULL;
    }

    return const_cast<Cell *>(&m_cells[x][y]);
}

void Board::onCellValueChanged() {
    Cell *cell = qobject_cast<Cell *>(sender());

    emit cellValueChanged(cell);

    if (cell->value() != 0) {

        bool rowFull = isRowFull(cell->y());
        if (rowFull)
            emit rowIsFull(cell->y());

        bool columnFull = isColumnFull(cell->x());
        if (columnFull)
            emit columnIsFull(cell->x());

        bool blockFull = isBlockFull(cell->block());
        if (blockFull)
            emit blockIsFull(cell->block());

        bool valueFull = isValueFull(cell->value());
        if (valueFull)
            emit valueIsFull(cell->value());

        if (rowFull && columnFull && blockFull && valueFull && isFull()) {
            pause();
            emit boardIsFull();
        }
    }
}

void Board::onBeforeCellValueChanged()
{
    Cell *cell = qobject_cast<Cell *>(sender());

    if (!blockModificationLog) {
        modificationLog.push(ModificationLogEntry(cell));

        if (modificationLog.size() == 1)
            emit canUndoChanged();
    }
}

QList<QObject *> Board::isValidMove(quint8 x, quint8 y, quint8 value) {
    QList<QObject *> list;

    isValidMoveHorizontal(x, y, value, list);
    isValidMoveVertical(x, y, value, list);
    isValidMoveBlock(x, y, value, list);

    return list;
}

void Board::isValidMoveHorizontal(quint8 x, quint8 y, quint8 val, QList<QObject *> &invalidList) const {
    for (quint8 i = 0; i < 9; i++) {
        if (i == x)
            continue;

        Cell *cell = cellAt(i, y);

        if (cell->value() == val && !invalidList.contains(cell))
            invalidList.append(cell);
    }
}

void Board::isValidMoveVertical(quint8 x, quint8 y, quint8 val, QList<QObject *> &invalidList) const {
    for (quint8 i = 0; i < 9; i++) {
        if (i == y)
            continue;

        Cell *cell = cellAt(x, i);

        if (cell->value() == val && !invalidList.contains(cell))
            invalidList.append(cell);
    }
}

void Board::isValidMoveBlock(quint8 x, quint8 y, quint8 val, QList<QObject *> &invalidList) const {
    quint8 startX = quint8(x / 3) * 3;
    quint8 startY = quint8(y / 3) * 3;

    for (quint8 yR = startY; yR < startY + 3; yR++) {
        for (quint8 xR = startX; xR < startX + 3; xR++) {
            if (xR == x && yR == y)
                continue;

            Cell *cell = cellAt(xR, yR);

            if (cell->value() == val && !invalidList.contains(cell))
                invalidList.append(cell);
        }
    }
}

bool Board::isFull() const {
    for (quint8 x = 0; x < 9; x++) {
        for (quint8 y = 0; y < 9; y++) {
            if (cellValue(x, y) == 0)
                return false;
        }
    }

    return true;
}

bool Board::isRowFull(int row) const {
    for (quint8 x = 0; x < 9; x++) {
        if (cellValue(x, row) == 0)
            return false;
    }

    return true;
}

bool Board::isColumnFull(int column) const {
    for (quint8 y = 0; y < 9; y++) {
        if (cellValue(column, y) == 0)
            return false;
    }

    return true;
}

bool Board::isBlockFull(int block) const {
    quint8 startX = quint8(block % 3) * 3;
    quint8 startY = quint8(block / 3) * 3;

    for (quint8 yR = startY; yR < startY + 3; yR++) {
        for (quint8 xR = startX; xR < startX + 3; xR++) {
            if (cellValue(xR, yR) == 0)
                return false;
        }
    }

    return true;
}

bool Board::isValueFull(int value) const {
    quint8 i = 0;
    for (quint8 y = 0; y < 9; y++) {
        for (quint8 x = 0; x < 9; x++) {
            if (cellValue(x, y) == value)
                i++;
        }
    }

    return i == 9;
}

QString Board::toString() const {
    QString output;

    for (quint8 x = 0; x < 9; x++) {
        for (quint8 y = 0; y < 9; y++) {
            output += QString("| ") + QString::number(cellValue(x, y)) + QString(" ");
        }
        output += " |\n";
        output += "|------------------------------------|\n";
    }

    return output;
}
void Board::clear() {
    for (quint8 x = 0; x < 9; x++) {
        for (quint8 y = 0; y < 9; y++) {
            cellAt(x, y)->clear();
        }
    }
}

quint8 Board::cellValue(quint8 x, quint8 y) const {
    return m_cellValues[x][y];
}

quint8 Board::solutionValue(quint8 x, quint8 y) const {
    return m_solution[x][y];
}

void Board::setCellValue(quint8 x, quint8 y, quint8 value) {
    if (m_cellValues[x][y] == value)
        return;

    Cell *cell = cellAt(x, y);

    // Check for collisions
    bool hasCollisions = false;
    if (value != 0) {
        for (quint8 y1 = 0; y1 < 9; y1++) {
            if (y1 == y)
                continue;

            if (m_cellValues[x][y1] == value) {
                emit cellAt(x, y1)->collides();
                hasCollisions = true;
            }
        }

        for (quint8 x1 = 0; x1 < 9; x1++) {
            if (x1 == x)
                continue;

            if (m_cellValues[x1][y] == value) {
                emit cellAt(x1, y)->collides();
                hasCollisions = true;
            }
        }

        for (quint8 x1 = (x / 3) * 3; x1 < ((x / 3) * 3) + 3; x1++) {
            for (quint8 y1 = (y / 3) * 3; y1 < ((y / 3) * 3) + 3; y1++) {
                if (x1 == x || y1 == y)
                    continue;

                if (m_cellValues[x1][y1] == value) {
                    emit cellAt(x1, y1)->collides();
                    hasCollisions = true;
                }
            }
        }
    }

    if (hasCollisions)
        return;

    emit cell->beforeValueChanged();

    m_cellValues[x][y] = value;
    if (value == 0)
        cell->setValueOwner(NULL);

    emit cell->valueChanged();

    if (value) {
        // clear corresponding notes in the same...
        for (quint8 i = 0; i < 9; ++i) {
            // row
            cellAt(i, y)->noteModel()->setMarked(value, false);
            // column
            cellAt(x, i)->noteModel()->setMarked(value, false);
        }

        // block
        quint8 startX = quint8(x / 3) * 3;
        quint8 startY = quint8(y / 3) * 3;
        for (quint8 yR = startY; yR < startY + 3; yR++) {
            for (quint8 xR = startX; xR < startX + 3; xR++)
                cellAt(xR, yR)->noteModel()->setMarked(value, false);
        }
    }
}

quint64 Board::startTime() const {
    return m_startTime;
}

quint64 Board::elapsedTime() const {
    // If we are paused return the elapsed time. The WindowActived event
    // is delivered after the elapsed time is requested which leads to
    // an incorrect display of the elapsed time.
    if (m_paused)
        return m_elapsedTime;

    return ((QDateTime::currentMSecsSinceEpoch() - m_startTime) + m_elapsedTime);
}

Cell *Board::selectedCell() const {
    return m_selectedCell;
}

void Board::setSelectedCell(Cell *cell) {
    if (m_selectedCell == cell)
        return;

    m_selectedCell = cell;

    emit selectedCellChanged();
}

void Board::pause() {
    if (m_paused)
        return;

    m_paused = true;
    m_elapsedTime += QDateTime::currentMSecsSinceEpoch() - m_startTime;
}

void Board::unpause() {
    if (!m_paused || isFull())
        return;

    m_paused = false;
    m_startTime = QDateTime::currentMSecsSinceEpoch();
}

void Board::undo() {
    if (modificationLog.isEmpty())
        return;

    ModificationLogEntry entry = modificationLog.pop();

    Cell *cell = cellAt(entry.x(), entry.y());

    blockModificationLog = true;
    cell->setValue(entry.value());
    cell->setValueOwner(entry.valueOwner());
    blockModificationLog = false;

    if (modificationLog.isEmpty())
        emit canUndoChanged();
}

bool Board::canUndo() const
{
    return !modificationLog.isEmpty();
}

Sudoku::Difficulty Board::difficulty() const
{
    return m_difficulty;
}
