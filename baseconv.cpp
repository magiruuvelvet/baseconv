#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cinttypes>
#include <iomanip>

#include <boost/algorithm/string.hpp>

std::string convert_base(const std::string &__number, int current_base, int new_base = 10)
{
  using INT_SIZE = std::intmax_t;
  using FRAC_SIZE = double;

  static const std::string digit_pattern = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  // Error message handler lamdba function.
  static const auto base_error = [] (std::string err_msg, int base_size = 0) {
    static const auto print_base_size = [] (int base_size) -> std::string {
      std::stringstream str; str << base_size;
      return (" (base size: " + str.str() + ")");
    };
    std::cout << "Base Converter: " << err_msg << (base_size != 0 ? print_base_size(base_size) : "") << std::endl;
  };

  // Do a security check with the provided bases and the number itself.
  if (current_base < 2 || current_base > 36 || new_base < 2 || new_base > 36) {
    base_error("Error in base size: Base must be between 2 and 36!");
    return std::string();
  }

  // Generate digit pattern for provided bases to convert between.
  const std::string digit_pattern_current_base = digit_pattern.substr(0, current_base) + '.';
  //const std::string digit_pattern_new_base     = digit_pattern.substr(0, new_base) + '.';

  // Convert number to upper case for number check.
  std::string number = boost::algorithm::to_upper_copy(__number);
  if (number.find_first_not_of(digit_pattern_current_base) != std::string::npos) {
    base_error("Error in number: Digits doesn't match given base size!", current_base);
    return std::string();
  }


  /// ===============================================================================
  ///  Begin of Base Converter

  // avoid conversion if source and target base match
  if (current_base == new_base)
    return number;

  // any to base-10 intern convert helper
  static const auto to_base10 = [&] (const std::string &__number__, int base) -> FRAC_SIZE {
    // Buffers and size parameters
    FRAC_SIZE result = 0, quot = 0, for_size = __number__.size();
    std::string number = __number__;
    std::string fraction;

    // Check if number has fraction/comma
    std::size_t comma_pos = number.find('.');
    if (comma_pos != std::string::npos) {
      number = number.substr(0, comma_pos);
      for_size = number.size();
      fraction = __number__.substr(comma_pos + 1);
    }

    // Convert integer number
    for (int i = 0; i < for_size; i++) {
      for (int j = 0; j < digit_pattern_current_base.size() - 1; j++) {
        if (number.at(0) == digit_pattern_current_base.at(j)) {
          quot = j; break;
        }
      } // inner for end

      result += quot * pow(base, number.size() - 1);
      number.erase(number.begin());
    } // outer for end

    // Convert fraction
    if (!fraction.empty()) {
      int pow_counter = -1;
      for_size = fraction.size();

      for (int i = 0; i < for_size; i++) {
        for (int j = 0; j < digit_pattern_current_base.size() - 1; j++) {
          if (fraction.at(0) == digit_pattern_current_base.at(j)) {
            quot = j; break;
          }
        } // inner for end

        result += quot * pow(base, pow_counter);
        fraction.erase(fraction.begin());
        pow_counter--;
      } // outer for end
    }

    number.clear();
    fraction.clear();
    return result;
  };

  if (new_base == 10) { // Only convert to decimal.
    std::stringstream str; str << to_base10(number, current_base);
    return str.str();
  } else {              // Convert to decimal and from decimal to any other base.
    FRAC_SIZE __temp_number;
    FRAC_SIZE fract = modf(to_base10(number, current_base), &__temp_number);
    INT_SIZE temp_number = static_cast<INT_SIZE>(__temp_number);

    // Convert integer part
    if (temp_number >= new_base) {
      std::imaxdiv_t div;
      std::list<INT_SIZE> rem;

      while (temp_number >= new_base) {
        div = std::imaxdiv(temp_number, new_base);
        temp_number = div.quot;
        rem.push_front(div.rem);
      } rem.push_front(div.quot);

      number.clear();
      for (INT_SIZE& i : rem)
        number.push_back(digit_pattern.at(i));
    } else {
      number.clear();
      number.push_back(digit_pattern.at(temp_number));
    }

    // Convert fraction part
    if (fract != 0.0f) {
      number.push_back('.');
      FRAC_SIZE dec_rem;
      INT_SIZE max_fract = 100; // Prevent an infinite loop on periodics.

      //fract * new_base <- decimal part -> remember;
      //result <- fractional part * new_base -> remember decimal part;
      //loop util 0 or periodic result;

      while (fract != 0.0f) {
        fract = fract * new_base;
        fract = modf(fract, &dec_rem);
        number.push_back(digit_pattern.at((INT_SIZE) dec_rem));

        max_fract--;
        if (max_fract == 0)
          break;
      }
    }
  }

  // Return new number
  return number;
}
