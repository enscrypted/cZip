
#ifndef QCA_EXPORT_H
#define QCA_EXPORT_H

#ifdef QCA_STATIC_DEFINE
#  define QCA_EXPORT
#  define QCA_NO_EXPORT
#else
#  ifndef QCA_EXPORT
#    ifdef QCA_MAKEDLL
        /* We are building this library */
#      define QCA_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define QCA_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef QCA_NO_EXPORT
#    define QCA_NO_EXPORT 
#  endif
#endif

#ifndef QCA_DEPRECATED
#  define QCA_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef QCA_DEPRECATED_EXPORT
#  define QCA_DEPRECATED_EXPORT QCA_EXPORT QCA_DEPRECATED
#endif

#ifndef QCA_DEPRECATED_NO_EXPORT
#  define QCA_DEPRECATED_NO_EXPORT QCA_NO_EXPORT QCA_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef QCA_NO_DEPRECATED
#    define QCA_NO_DEPRECATED
#  endif
#endif

#endif /* QCA_EXPORT_H */
