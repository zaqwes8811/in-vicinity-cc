http://www.drdobbs.com/cpp/c-compilation-speed/228701711
http://stackoverflow.com/questions/318398/why-does-c-compilation-take-so-long
http://blog.8thlight.com/dariusz-pasciak/2012/08/14/c-plus-plus-is-not-slow.html



\fixme incapsulate thread id's
Static and global - lifetime troubles

\attention Real trouble with checking current thread, not dispatch

все объекты, что передается в PostTask - должно быть RefCounted

Theory actors
Goods:
"Многопоточность в браузере. Модель акторов"

Tricks:
https:vimeo.com/46276948?ref=tw-share - Doing it wrong - not for all!!
http://james-iry.blogspot.co.uk/2009/04/erlang-style-actors-are-all-about.html - non stateless really

From Sutter:
http://www.drdobbs.com/parallel/prefer-using-active-objects-instead-of-n/225700095
http://www.drdobbs.com/cpp/prefer-using-futures-or-callbacks-to-com/226700179
http://www.drdobbs.com/architecture-and-design/know-when-to-use-an-active-object-instea/227500074?pgno=3
http://www.chromium.org/developers/design-documents/threading

http://stackoverflow.com/questions/19192122/template-declaration-of-typedef-typename-footbar-bar
typedef
template <typename T>
boost::shared_ptr<boost::packaged_task<T> > Task;

What is wrong:
- проблемы с разрушениме
- нет подсчета ссылок
