#if defined(TOUCH_FT6236U) || defined(TOUCH_FT6336) || defined(TOUCH_CST816) || defined(TOUCH_GT911) || defined(TOUCH_XPT2046) || defined(TOUCH_GSL3680)
#define HAS_TOUCH true
#else
#undef HAS_TOUCH
#endif