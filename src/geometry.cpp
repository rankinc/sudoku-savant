#include <sstream>
using namespace std;

#include "geometry.h"
#include "strip_blanks.h"

namespace sudoku
{

/*********************************************************************
 */
Geometry::Geometry(size_type boxWidth, size_type boxHeight)
  : m_maxNumber( getMaxNumber(boxWidth, boxHeight) )
  , m_boxWidth(boxWidth)
  , m_boxHeight(boxHeight)
{
  verify(m_maxNumber);
}


/*********************************************************************
 */
void
Geometry::setBoxWidth(size_type w)
{
  m_maxNumber = verify(m_boxHeight * w);
  m_boxWidth = w;
}

/*********************************************************************
 */
void
Geometry::setBoxHeight(size_type h)
{
  m_maxNumber = verify(m_boxWidth * h);
  m_boxHeight = h;
}

/*********************************************************************
 */
Geometry::size_type
Geometry::verify(size_type maximum)
{
  if (maximum > MAX_BOX_SIZE)
  {
    ostringstream ostr;
    ostr << "Box too large: maximum of "
         << MAX_BOX_SIZE
         << " distinct numbers.";
    throw runtime_error(ostr.str());
  }

  return maximum;
}

/*********************************************************************
 */
bool
Geometry::operator==(const Geometry &g) const
{
  return ( (g.getBoxWidth() == getBoxWidth()) &&
           (g.getBoxHeight() == getBoxHeight()) );
}

}

namespace std
{

/*********************************************************************
 */
istream&
operator>>(istream &istr, sudoku::Geometry &geometry)
{
  const streampos pos = istr.tellg();
  bool failed = true;
  string line;

  if ( !sudoku::getLine(istr, line).empty() )
  {
    sudoku::StringPair option;

    if ( sudoku::getOption(line, option) &&
         (option.name.compare("GEOMETRY") == 0) )
    {
      istringstream is(option.value);

      sudoku::Geometry::size_type width, height;
      char c;

      is >> width >> ws >> c >> ws >> height;
      if (is && (width != 0) && (height != 0))
      {
        geometry.setBoxWidth(width);
        geometry.setBoxHeight(height);
        failed = false;
      }
    }
  }

  if (failed)
    istr.seekg(pos, ios::beg);

  return istr;
}

/*********************************************************************
 */
ostream&
operator<<(ostream &ostr, const sudoku::Geometry &geometry)
{
  ostr << geometry.getBoxWidth()
       << "x"
       << geometry.getBoxHeight();
  return ostr;
}

}

