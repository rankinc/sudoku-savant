#ifndef ACTIONS_H
#define ACTIONS_H

#include <functional>

namespace sudoku
{

/*********************************************************************
 */
template<class T>
class Action : public std::unary_function<T, void>
{
public:
  virtual void operator()(T &x) const = 0;
  virtual ~Action() {}
};


template<class T>
class RefAction : public std::unary_function<T, void>
{
public:
  virtual void operator()(T *x) const = 0;
  virtual ~RefAction() {}
};


/*********************************************************************
 */
template<class T>
class Clear : public Action<T>
{
public:
  void operator()(T &x) const { x.clear(); }
};


/*********************************************************************
 */
template<class T>
class DropStack : public Action<T>
{
public:
  void operator()(T &x) const { x.dropStack(); }
};


/*********************************************************************
 */
template<class T>
class BoolAct : public std::unary_function<T, bool>
{
private:
  typedef bool(T::*func)(unsigned);

  func     m_func;
  unsigned m_number;

public:
  BoolAct(func f, unsigned n) : m_func(f), m_number(n) {}
  bool operator()(T *x) const { return (x->*m_func)(m_number); }
};


/*********************************************************************
 */
template<class T>
class Delete : public Action<T>
{
public:
  void operator()(T &x) const { delete x; }
};


/*********************************************************************
 */
template<typename Iterator, typename Action>
void
do_for_each(Iterator first, const Iterator &last, const Action &action)
{
  while (first != last)
  {
    action(*first);
    ++first;
  } /* while */
}

}

#endif

