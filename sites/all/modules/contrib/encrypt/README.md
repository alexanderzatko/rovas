# CONTENTS OF THIS FILE

* Introduction
* Requirements
* Recommended modules
* Installation
* Configuration


# INTRODUCTION

This module is an API that other modules can use to encrypt data. It
doesn't provide any user-facing features of its own, aside from
administration pages to manage configuration.

#REQUIREMENTS

* [Chaos Tool Suite][1]:  
  All encryption methods and key providers are implemented as **Chaos
  Tool Suite** plugins.

#RECOMMENDED MODULES

* [Advanced Help Hint][5]:  
  Links help text provided by `hook_help` to online help and
  **Advanced Help**.
* [Advanced Help][4]:  
  When this module is enabled, additional on screen help will be
  available.
* [Markdown][6]:  
  When this module is enabled, display of the project's `README.md`
  will be rendered with the markdown filter.

#INSTALLATION

Install as you would normally install a contributed drupal
 module. See: [Installing modules][7] for further information.

#CONFIGURATION

**Encrypt** allows multiple configurations to be managed within a
Drupal site. Each configuration contains an encryption method and a
key provider, along with any additional settings that the method or
provider requires.

The [advanced help](/admin/help/ah/encrypt) framework provides more
information (if it is enabled for the site).

[1]: https://www.drupal.org/project/ctools
[4]: https://www.drupal.org/project/advanced_help
[5]: https://www.drupal.org/project/advanced_help_hint
[6]: https://www.drupal.org/project/markdown
[7]: https://drupal.org/documentation/install/modules-themes/modules-7
