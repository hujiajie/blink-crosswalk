/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2006 Apple Computer, Inc
 * Copyright (C) 2009 Google, Inc.
 * Copyright (C) 2011 Renata Hodovan <reni@webkit.org>
 * Copyright (C) 2011 University of Szeged
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#ifndef LayoutSVGPath_h
#define LayoutSVGPath_h

#include "core/layout/svg/LayoutSVGShape.h"

namespace blink {

class LayoutSVGPath final : public LayoutSVGShape {
public:
    explicit LayoutSVGPath(SVGGraphicsElement*);
    virtual ~LayoutSVGPath();

    virtual const Vector<MarkerPosition>* markerPositions() const override { return &m_markerPositions; }

    virtual const Vector<FloatPoint>* zeroLengthLineCaps() const override { return &m_zeroLengthLinecapLocations; }
    static FloatRect zeroLengthSubpathRect(const FloatPoint&, float);

private:
    virtual const char* renderName() const override { return "LayoutSVGPath"; }

    virtual void updateShapeFromElement() override;
    FloatRect calculateUpdatedStrokeBoundingBox() const;

    virtual bool shapeDependentStrokeContains(const FloatPoint&) override;

    FloatRect markerRect(float strokeWidth) const;
    bool shouldGenerateMarkerPositions() const;
    void processMarkerPositions();

    bool shouldStrokeZeroLengthSubpath() const;
    void updateZeroLengthSubpaths();

    Vector<MarkerPosition> m_markerPositions;
    Vector<FloatPoint> m_zeroLengthLinecapLocations;
};

}

#endif
