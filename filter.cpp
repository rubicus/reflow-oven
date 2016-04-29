#include "filter.hpp"

// this class will expect 16 bit inputs and outputs, but store 32 bit values
// internally. It's a 4 tap IIR filter following this pattern:

const short init_val = 20 << 6;

// y(n) = a0*x(n) + a1*x(n-1) + a2*x(n-2) + b1*y(n-1) + b2*y(n-1)
// step is the number of steps results should be shifted after multiplyting
// it's a way of "scaling" the contsants.
// const = val*2^(step) Ex: val= 1.5, step = 14 => const = 24567
// step = 15 gives val range [-1..1), 14 [-2..2) etc
class Filter {
public:
  Filter(short a0, short a1, short a2, short b1, short b2,
         char step) :
    a0(a0), a1(a1), a2(a2), b1(b1), b2(b2), step(step),
    x1(init_val), x2(init_val), y0(init_val), y1(init_val) {}
  short new_input(short x0);
  short get_last() {return y0;}
private:
  const short a0, a1, a2;
  const short b1, b2;
  const char step;
  short x1, x2;
  short y0, y1;
};

short Filter::new_input(short x0) {
  long sum = 0;
  short y2 = y1;
  y1 = y0;

  sum += long(a0) * x0;
  sum += long(a1) * x1;
  sum += long(a2) * x2;
  sum += long(b1) * y1;
  sum += long(b2) * y2;

  y0 = short (sum >> step);

  //shift the x:es
  x2 = x1;
  x1 = x0;
  return y0;
}

// here we implement two filters in series. We use step size 14 and the
// following values:
// First:  0,044, 0,089, 0,044, 1,459, -0,557
// second: 0,031, 0,063, 0,031, 1,567, -0,795
class Double_filter {
public:
  Double_filter() :
    first_filter(720,1441,720,23903, -9128, 14),
    second_filter(507, 1032, 507,25676, -13033, 14) {}
  short write_value(short new_val);
  short get_last();
private:
  Filter first_filter;
  Filter second_filter;
};

Double_filter temp_filter;

short Double_filter::write_value(short new_val) {
  short intermediate_val = first_filter.new_input(new_val);
  short return_val = second_filter.new_input(intermediate_val);
  return return_val;
}

short Double_filter::get_last() {
  return second_filter.get_last();
}

short write_filter_value(short new_val) {
  return temp_filter.write_value(new_val);
}

short get_last_val() {
  return temp_filter.get_last();
}
