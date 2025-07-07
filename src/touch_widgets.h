#include "../user_setup.h"
#include "widgets/widgetsetup.h"

#if HAS_TOUCH


#if defined(DFK_TOUCHAREA)
#include "widgets/toucharea/wtoucharea.h"
#endif

#ifdef DFK_CHECKBOX
#include "widgets/checkbox/wcheckbox.h"
#endif

#ifdef DFK_CIRCLEBTN
#include "widgets/circlebutton/wcirclebutton.h"
#endif

#ifdef DFK_HSLIDER
#include "widgets//hslider/whslider.h"
#endif

#ifdef DFK_RADIO
#include "widgets/radiobutton/wradiogroup.h"
#endif

#ifdef DFK_RECTBTN
#include "widgets/rectbutton/wrectbutton.h"
#endif

#ifdef DFK_TOGGLE
#include "widgets/togglebutton/wtoggle.h"
#endif

#ifdef DFK_TEXTBOX
#include "widgets/textbox/wtextbox.h"
#include "widgets/textbox/wkeyboard.h"
#endif

#ifdef DFK_NUMBERBOX
#include "widgets/numberbox/wnumberbox.h"
#include "widgets/numberbox/numpad.h"
#endif

#ifdef DFK_TEXTBUTTON
#include "widgets/textbutton/wtextbutton.h"
#endif

#ifdef DFK_SPINBOX
#include "widgets/spinbox/wspinbox.h"
#endif


#else

#undef DFK_TOUCHAREA
#undef DFK_CHECKBOX
#undef DFK_CIRCLEBTN
#undef DFK_HSLIDER
#undef DFK_RADIO
#undef DFK_RECTBTN
#undef DFK_TOGGLE
#undef DFK_TEXTBOX
#undef DFK_NUMBERBOX
#undef DFK_TEXTBUTTON
#undef DFK_SPINBOX

#endif