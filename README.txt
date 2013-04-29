Infusionsoft API module

What is the infusionsoft API module?
---------

This module acts as a layer between Drupal and infusionsoft, which makes the process of talking between the two much easier.

Why is this module needed?
Infusionsoft is a proprietary CRM, and highly available email auto responder system. This module, makes things such as adding a user to infusionsoft, or manipulating user information being sent to infusionsoft easier, by providing default functions for common actions.

Also, this module is faster than using the Infusionsoft SDK because it uses the built in drupal version of XML-RPC instead of using it twice.

This module also includes drupal specific functions for things like adding a drupal user to infusionsoft.

Default Integrations

Ubercart conditional actions
Provides configurable conditional actions for use within ubercart

Drupal actions
Provides configurable actions which can be used by drupal triggers module, and other modules which leverage the drupal actions system.

Rules
works with the rules module to provide a tags condition, rules can also leverage drupal actions to perform various functions.

Submodules

infusionsoft_log_email
A small module when enabled will log all outgoing drupal email to the receiving contact's infusionsoft correspondence record.

infusionsoft_oneway_sync
Synching an email address to infusionsoft when someone updates their profile.

infusionsoft_uc_access
Conditional action and api function to sync contact data from uc_adresses to infusionsoft

Install
----------

To install read the INSTALL.txt file

Use
-------------
This module provides the following actions for use in the system through the built in trigger module, or through the rules module

* Add current user to infusionsoft database
* Add tag to drupal user in the infusionsoft database
* Remove tag from a drupal user in the infusionsoft database
* Run an action set on a user in the infusionsoft database
* Add user to a campaign in the infusionsoft database
* Remove user from a campaign in the infusionsoft database
* Pause a campaign for a user in the infusionsoft database
* Resume a campaign for a user in the infusionsoft database

Rules only:
Condition: User has a specific infusionsoft tag.

How does this differ than the 'infusionlink' module?
_____________

This module takes some of the ideas from this module, and makes it much more extensible. It is much more robust, because you can actually set up a site without any coding by using drupal actions or ubercart.

Special thanks to this thread (and its contributors)
http://drupal.org/node/922964

Developers
_____________

This module allows developers to leverage infusionsoft without reinventing the wheel. Do this by using the services and methods found here:
http://help.infusionsoft.com/developers/services-methods
and encapsulating them by calling the service and method as the first variable passed to infusionsoft_send_request(). All subsequent variables should be passed following this first request. You should not include the API key in this request.

Also There is an infusionsoft api built especially for drupal. It is documented inside the module in the infusionsoft.api.inc file.

You need to know which service to use. method calls look like this:
Service.method (as documented http://help.infusionsoft.com/developers/services-methods)

The code in the infusionsoft module is thoroughly documented to make it easy to use, just look through infusionsoft.api.inc to find more descriptive functions to use in your own application.


For the clearly documented API usage see infusionsoft.module and infusionsoft.api.inc files.
