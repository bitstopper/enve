#include "custompatheffect.h"
#include "basicreadwrite.h"
CustomPathEffect::CustomPathEffect(const QString &name,
                                   const bool outlinePathEffect) :
    PathEffect(name, CUSTOM_PATH_EFFECT, outlinePathEffect) {

}

void CustomPathEffect::writeProperty(QIODevice * const target) const {
    PathEffect::writeProperty(target);
    const auto identifier = getIdentifier();
    const int size = identifier.size();
    target->write(rcConstChar(&size), sizeof(int));
    target->write(identifier);
    write(target);
}

void CustomPathEffect::readProperty(QIODevice *target) {
    PathEffect::readProperty(target);
    int size;
    target->read(rcChar(&size), sizeof(int));
    const QByteArray identifier = target->read(size);
    read(identifier, target);
}
