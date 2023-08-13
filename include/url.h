#ifndef __GWEB_URL_H__
#define __GWEB_URL_H__

/// process a url. Depending on the input this function returns output that
/// potentially has to be cleaned up by the user. If the function doesn't
/// process the url it returns its input. So you can just compare it to it's
/// input.
char *process_url(char *in);

#endif
