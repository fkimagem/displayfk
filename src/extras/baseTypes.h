
#if !defined(BASE_TYPES_H)

#include <stdint.h>

#define BASE_TYPES_H

//#define CLAMP(cvalue, vmin, vmax) ((cvalue) < (vmin) ? (vmin) : ((cvalue) > (vmax) ? (vmax) : (cvalue))) ///< Trucate value between min and max
#define TIMEOUT_CLICK 500
#define DGTORAD(value) ((value) * 0.0174532925)


/// @brief Typedef for a generic callback function.
typedef void(*functionCB_t)(); ///< Define callback function


/// @brief Typedef for a callback function used to load screens on request.
typedef void(*functionLoadScreen_t)(); ///< Define callback to load screen when solicited


/// @brief Structure representing a 2D point with X and Y coordinates.
typedef struct
{
  uint16_t x = 0; ///< X coordinate.
  uint16_t y = 0; ///< Y coordinate.
} CoordPoint_t;


/// @brief Structure representing a rectangle with position and size.
typedef struct
{
  uint16_t x = 0;      ///< X coordinate of the rectangle.
  uint16_t y = 0;      ///< Y coordinate of the rectangle.
  uint16_t width = 0;  ///< Width of the rectangle.
  uint16_t height = 0; ///< Height of the rectangle.
} Rect_t;


/// @brief Struct to save text bounding box data.
typedef struct
{
  int16_t x = 0;       ///< X coordinate of the text bounding box.
  int16_t y = 0;       ///< Y coordinate of the text bounding box.
  uint16_t width = 0;  ///< Width of the text bounding box.
  uint16_t height = 0; ///< Height of the text bounding box.
} TextBound_t;


/// @brief Enumeration for different font types.
enum class FontType
{
  UNLOAD, ///< No font loaded.
  NORMAL, ///< Normal font type.
  BOLD    ///< Bold font type.
};

#endif

