#include "board.h"
#include "player.h"

Board::Board(const Board &other) :
    QObject(other.parent()) {

    memcpy(m_cellValues, other.m_cellValues, sizeof(m_cellValues));

    for (quint8 x = 0; x < 9; x++) {
        for (quint8 y = 0; y < 9; y++) {
            m_cells[x][y] = other.m_cells[x][y];
            m_cells[x][y].setParent(this);

            connect(&m_cells[x][y], SIGNAL(valueChanged()), SLOT(onCellValueChanged()));
        }
    }
}

Board::Board(QObject *parent) :
    QObject(parent)
{
    for (quint8 y = 0; y < 9; y++) {
        for (quint8 x = 0; x < 9; x++) {
            m_cellValues[x][y] = 0;
            m_cells[x][y].m_x = x;
            m_cells[x][y].m_y = y;
            m_cells[x][y].setParent(this);

            connect(&m_cells[x][y], SIGNAL(valueChanged()), SLOT(onCellValueChanged()));
        }
    }
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

    if (!isFull())
        emit boardIsFull();
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

void Board::setCellValue(quint8 x, quint8 y, quint8 value) {
    m_cellValues[x][y] = value;
}

QDataStream &operator<<(QDataStream &s, const Board &board) {
    for (quint8 y = 0; y < 9; y++) {
        for (quint8 x = 0; x < 9; x++) {
            Cell *cell = board.cellAt(x, y);

            QUuid uuid;

            if (cell->valueOwner() != NULL)
                uuid = cell->valueOwner()->uuid();

            s << uuid;
            s << cell->isFixedCell();
            s << cell->value();
        }
    }

    return s;
}

QDataStream &operator>>(QDataStream &s, Board &board) {
    for (quint8 y = 0; y < 9; y++) {
        for (quint8 x = 0; x < 9; x++) {
            QUuid uuid;

            s >> uuid;
            s >> board.cellAt(x, y)->m_fixedCell;
            s >> board.m_cellValues[x][y];
        }
    }

    return s;
}

Cell::Cell(quint8 x, quint8 y, Board *parent) :
    QObject(parent), m_x(x), m_y(y), m_fixedCell(false), m_valueOwner(NULL) {
}

quint8 Cell::value() const {
    return board()->m_cellValues[m_x][m_y];
}

void Cell::setValue(quint8 value) {
    if (this->value() == value)
        return;

    board()->setCellValue(m_x, m_y, value);

    if (value == 0)
        setValueOwner(NULL);

    emit valueChanged();
}

void Cell::setValueOwner(Player *player) {
    if (player == m_valueOwner || (player && m_valueOwner && player == m_valueOwner))
        return;

    m_valueOwner = player;

    emit valueOwnerChanged();
}

void Cell::clear() {
    setValue(0);
    m_valueOwner = NULL;
}

Cell &Cell::operator =(const Cell &other) {
    m_x = other.m_x;
    m_y = other.m_y;
    m_fixedCell = other.m_fixedCell;
    m_valueOwner = other.m_valueOwner;

    return *this;
}
