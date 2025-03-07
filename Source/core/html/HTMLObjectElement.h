/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2004, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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
 *
 */

#ifndef HTMLObjectElement_h
#define HTMLObjectElement_h

#include "core/html/FormAssociatedElement.h"
#include "core/html/HTMLPlugInElement.h"

namespace blink {

class HTMLFormElement;

class HTMLObjectElement final : public HTMLPlugInElement, public FormAssociatedElement {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(HTMLObjectElement);
public:
    static PassRefPtrWillBeRawPtr<HTMLObjectElement> create(Document&, HTMLFormElement*, bool createdByParser);
    virtual ~HTMLObjectElement();
    DECLARE_VIRTUAL_TRACE();

    const String& classId() const { return m_classId; }

    virtual HTMLFormElement* formOwner() const override;

    bool containsJavaApplet() const;

    virtual bool hasFallbackContent() const override;
    virtual bool useFallbackContent() const override;
    virtual void renderFallbackContent() override;

    virtual bool isFormControlElement() const override { return false; }

    virtual bool isEnumeratable() const override { return true; }
    virtual bool isInteractiveContent() const override;
    virtual bool appendFormData(FormDataList&, bool) override;

    virtual bool isObjectElement() const override { return true; }

    // Implementations of constraint validation API.
    // Note that the object elements are always barred from constraint validation.
    virtual String validationMessage() const override { return String(); }
    bool checkValidity() { return true; }
    bool reportValidity() { return true; }
    virtual void setCustomValidity(const String&) override { }

#if !ENABLE(OILPAN)
    using Node::ref;
    using Node::deref;
#endif

    virtual bool canContainRangeEndPoint() const override { return useFallbackContent(); }

    bool isExposed() const;

private:
    HTMLObjectElement(Document&, HTMLFormElement*, bool createdByParser);

    virtual void parseAttribute(const QualifiedName&, const AtomicString&) override;
    virtual bool isPresentationAttribute(const QualifiedName&) const override;
    virtual void collectStyleForPresentationAttribute(const QualifiedName&, const AtomicString&, MutableStylePropertySet*) override;

    virtual InsertionNotificationRequest insertedInto(ContainerNode*) override;
    virtual void removedFrom(ContainerNode*) override;

    virtual bool rendererIsNeeded(const LayoutStyle&) override;
    virtual void didMoveToNewDocument(Document& oldDocument) override;

    virtual void childrenChanged(const ChildrenChange&) override;

    virtual bool isURLAttribute(const Attribute&) const override;
    virtual bool hasLegalLinkAttribute(const QualifiedName&) const override;
    virtual const QualifiedName& subResourceAttributeName() const override;
    virtual const AtomicString imageSourceURL() const override;

    virtual LayoutPart* existingLayoutPart() const override;

    virtual void updateWidgetInternal() override;
    void updateDocNamedItem();

    void reattachFallbackContent();

    // FIXME: This function should not deal with url or serviceType
    // so that we can better share code between <object> and <embed>.
    void parametersForPlugin(Vector<String>& paramNames, Vector<String>& paramValues, String& url, String& serviceType);

    bool hasValidClassId();

    void reloadPluginOnAttributeChange(const QualifiedName&);

#if !ENABLE(OILPAN)
    virtual void refFormAssociatedElement() override { ref(); }
    virtual void derefFormAssociatedElement() override { deref(); }
#endif

    virtual bool shouldRegisterAsNamedItem() const override { return true; }
    virtual bool shouldRegisterAsExtraNamedItem() const override { return true; }

    String m_classId;
    bool m_useFallbackContent : 1;
};

// Intentionally left unimplemented, template specialization needs to be provided for specific
// return types.
template<typename T> inline const T& toElement(const FormAssociatedElement&);
template<typename T> inline const T* toElement(const FormAssociatedElement*);

// Make toHTMLObjectElement() accept a FormAssociatedElement as input instead of a Node.
template<> inline const HTMLObjectElement* toElement<HTMLObjectElement>(const FormAssociatedElement* element)
{
    ASSERT_WITH_SECURITY_IMPLICATION(!element || !element->isFormControlElement());
    ASSERT_WITH_SECURITY_IMPLICATION(!element || !element->isLabelElement());
    const HTMLObjectElement* objectElement = static_cast<const HTMLObjectElement*>(element);
    // We need to assert after the cast because FormAssociatedElement doesn't
    // have hasTagName.
    ASSERT_WITH_SECURITY_IMPLICATION(!objectElement || objectElement->hasTagName(HTMLNames::objectTag));
    return objectElement;
}

template<> inline const HTMLObjectElement& toElement<HTMLObjectElement>(const FormAssociatedElement& element)
{
    ASSERT_WITH_SECURITY_IMPLICATION(!element.isFormControlElement());
    ASSERT_WITH_SECURITY_IMPLICATION(!element.isLabelElement());
    const HTMLObjectElement& objectElement = static_cast<const HTMLObjectElement&>(element);
    // We need to assert after the cast because FormAssociatedElement doesn't
    // have hasTagName.
    ASSERT_WITH_SECURITY_IMPLICATION(objectElement.hasTagName(HTMLNames::objectTag));
    return objectElement;
}

} // namespace blink

#endif // HTMLObjectElement_h
