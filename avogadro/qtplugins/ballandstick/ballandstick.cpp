/******************************************************************************

  This source file is part of the Avogadro project.

  Copyright 2012 Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/

#include "ballandstick.h"

#include <avogadro/core/molecule.h>
#include <avogadro/core/elements.h>
#include <avogadro/rendering/geometrynode.h>
#include <avogadro/rendering/groupnode.h>
#include <avogadro/rendering/spheregeometry.h>
#include <avogadro/rendering/cylindergeometry.h>
#include <avogadro/qtgui/rwmolecule.h>

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

namespace Avogadro {
namespace QtPlugins {

using Core::Elements;
using Core::Molecule;
using Rendering::GeometryNode;
using Rendering::GroupNode;
using Rendering::SphereGeometry;
using Rendering::CylinderGeometry;

BallAndStick::BallAndStick(QObject *p) : ScenePlugin(p), m_enabled(true),
  m_setupWidget(NULL)
{
}

BallAndStick::~BallAndStick()
{
  if (m_setupWidget)
    m_setupWidget->deleteLater();
}

void BallAndStick::process(const Molecule &molecule,
                           Rendering::GroupNode &node)
{
  // Add a sphere node to contain all of the spheres.
  GeometryNode *geometry = new GeometryNode;
  node.addChild(geometry);
  SphereGeometry *spheres = new SphereGeometry;
  spheres->identifier().molecule = &molecule;
  spheres->identifier().type = Rendering::AtomType;
  geometry->addDrawable(spheres);

  for (Index i = 0; i < molecule.atomCount(); ++i) {
    Core::Atom atom = molecule.atom(i);
    unsigned char atomicNumber = atom.atomicNumber();
    const unsigned char *c = Elements::color(atomicNumber);
    Vector3ub color(c[0], c[1], c[2]);
    spheres->addSphere(atom.position3d().cast<float>(), color,
                       static_cast<float>(Elements::radiusVDW(atomicNumber))
                       * 0.3f);
  }

  float bondRadius = 0.1f;
  CylinderGeometry *cylinders = new CylinderGeometry;
  cylinders->identifier().molecule = &molecule;
  cylinders->identifier().type = Rendering::BondType;
  geometry->addDrawable(cylinders);
  for (Index i = 0; i < molecule.bondCount(); ++i) {
    Core::Bond bond = molecule.bond(i);
    Vector3f pos1 = bond.atom1().position3d().cast<float>();
    Vector3f pos2 = bond.atom2().position3d().cast<float>();
    Vector3ub color1(Elements::color(bond.atom1().atomicNumber()));
    Vector3ub color2(Elements::color(bond.atom2().atomicNumber()));
    Vector3f bondVector = pos2 - pos1;
    float bondLength = bondVector.norm();
    bondVector /= bondLength;
    switch (bond.order()) {
    case 3: {
      Vector3f delta = bondVector.unitOrthogonal() * (2.0f * bondRadius);
      cylinders->addCylinder(pos1 + delta, bondVector, bondLength, bondRadius,
                             color1, color2, i);
      cylinders->addCylinder(pos1 - delta, bondVector, bondLength, bondRadius,
                             color1, color2, i);
    }
    default:
    case 1:
      cylinders->addCylinder(pos1, bondVector, bondLength, bondRadius,
                             color1, color2, i);
      break;
    case 2: {
      Vector3f delta = bondVector.unitOrthogonal() * bondRadius;
      cylinders->addCylinder(pos1 + delta, bondVector, bondLength, bondRadius,
                             color1, color2, i);
      cylinders->addCylinder(pos1 - delta, bondVector, bondLength, bondRadius,
                             color1, color2, i);
    }
    }
  }
}

void BallAndStick::processEditable(const QtGui::RWMolecule &molecule,
                                   Rendering::GroupNode &node)
{
  // Add a sphere node to contain all of the spheres.
  GeometryNode *geometry = new GeometryNode;
  node.addChild(geometry);
  SphereGeometry *spheres = new SphereGeometry;
  spheres->identifier().molecule = &molecule;
  spheres->identifier().type = Rendering::AtomType;
  geometry->addDrawable(spheres);

  for (Index i = 0; i < molecule.atomCount(); ++i) {
    QtGui::RWAtom atom = molecule.atom(i);
    unsigned char atomicNumber = atom.atomicNumber();
    const unsigned char *c = Elements::color(atomicNumber);
    Vector3ub color(c[0], c[1], c[2]);
    spheres->addSphere(atom.position3d().cast<float>(), color,
                       static_cast<float>(Elements::radiusVDW(atomicNumber))
                       * 0.3f);
  }

  float bondRadius = 0.1f;
  CylinderGeometry *cylinders = new CylinderGeometry;
  cylinders->identifier().molecule = &molecule;
  cylinders->identifier().type = Rendering::BondType;
  geometry->addDrawable(cylinders);
  for (Index i = 0; i < molecule.bondCount(); ++i) {
    QtGui::RWBond bond = molecule.bond(i);
    Vector3f pos1 = bond.atom1().position3d().cast<float>();
    Vector3f pos2 = bond.atom2().position3d().cast<float>();
    Vector3ub color1(Elements::color(bond.atom1().atomicNumber()));
    Vector3ub color2(Elements::color(bond.atom2().atomicNumber()));
    Vector3f bondVector = pos2 - pos1;
    float bondLength = bondVector.norm();
    bondVector /= bondLength;
    switch (bond.order()) {
    case 3: {
      Vector3f delta = bondVector.unitOrthogonal() * (2.0f * bondRadius);
      cylinders->addCylinder(pos1 + delta, bondVector, bondLength, bondRadius,
                             color1, color2, i);
      cylinders->addCylinder(pos1 - delta, bondVector, bondLength, bondRadius,
                             color1, color2, i);
    }
    default:
    case 1:
      cylinders->addCylinder(pos1, bondVector, bondLength, bondRadius,
                             color1, color2, i);
      break;
    case 2: {
      Vector3f delta = bondVector.unitOrthogonal() * bondRadius;
      cylinders->addCylinder(pos1 + delta, bondVector, bondLength, bondRadius,
                             color1, color2, i);
      cylinders->addCylinder(pos1 - delta, bondVector, bondLength, bondRadius,
                             color1, color2, i);
    }
    }
  }
}

bool BallAndStick::isEnabled() const
{
  return m_enabled;
}

void BallAndStick::setEnabled(bool enable)
{
  m_enabled = enable;
}

QWidget * BallAndStick::setupWidget()
{
  if (!m_setupWidget) {
    m_setupWidget = new QWidget(qobject_cast<QWidget*>(parent()));
    QVBoxLayout *v = new QVBoxLayout;
    QLabel *label = new QLabel("Test Ball and Stick");
    v->addWidget(label);
    m_setupWidget->setLayout(v);
  }
  return m_setupWidget;
}

}
}
