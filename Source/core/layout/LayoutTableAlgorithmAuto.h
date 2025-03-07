/*
 * Copyright (C) 2002 Lars Knoll (knoll@kde.org)
 *           (C) 2002 Dirk Mueller (mueller@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef LayoutTableAlgorithmAuto_h
#define LayoutTableAlgorithmAuto_h

#include "core/layout/LayoutTableAlgorithm.h"
#include "platform/LayoutUnit.h"
#include "platform/Length.h"
#include "wtf/Vector.h"

namespace blink {

class LayoutTable;
class LayoutTableCell;

class LayoutTableAlgorithmAuto final : public LayoutTableAlgorithm {
public:
    LayoutTableAlgorithmAuto(LayoutTable*);
    virtual ~LayoutTableAlgorithmAuto();

    virtual void computeIntrinsicLogicalWidths(LayoutUnit& minWidth, LayoutUnit& maxWidth) override;
    virtual void applyPreferredLogicalWidthQuirks(LayoutUnit& minWidth, LayoutUnit& maxWidth) const override;
    virtual void layout() override;
    virtual void willChangeTableLayout() override { }

private:
    void fullRecalc();
    void recalcColumn(unsigned effCol);

    int calcEffectiveLogicalWidth();

    void insertSpanCell(LayoutTableCell*);

    struct Layout {
        Layout()
            : minLogicalWidth(0)
            , maxLogicalWidth(0)
            , effectiveMinLogicalWidth(0)
            , effectiveMaxLogicalWidth(0)
            , computedLogicalWidth(0)
            , emptyCellsOnly(true)
        {
        }

        Length logicalWidth;
        Length effectiveLogicalWidth;
        int minLogicalWidth;
        int maxLogicalWidth;
        int effectiveMinLogicalWidth;
        int effectiveMaxLogicalWidth;
        int computedLogicalWidth;
        bool emptyCellsOnly;
    };

    Vector<Layout, 4> m_layoutStruct;
    Vector<LayoutTableCell*, 4> m_spanCells;
    bool m_hasPercent : 1;
    mutable bool m_effectiveLogicalWidthDirty : 1;
};

} // namespace blink

#endif // LayoutTableAlgorithmAuto
