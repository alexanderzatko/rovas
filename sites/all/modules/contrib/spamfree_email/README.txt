## Introduction

Spamfree Email module works on Javascript enabled and disabled browsers
preventing email harvesting. The Email address given in content type fields
like (body, summary etc,) are converted into corresponding mailto links and
provides email obfuscation to protect email addresses from harvesting bots. A 
javascript array of the original email address is generated and is parsed back 
to HTML, this way it is shown in Javascript-enabled browser, at the same time 
for non JS browsers an image of the email address (using Gd library) is 
generated and displayed. This prevents non-javascript capable bots from 
harvesting email addresses. Every time the email generated javascript array 
will change making it difficult for the bots to do any guess work. Spamfree 
Email does not use any common css class names or ids making it further 
difficult for the bots to grab the text within that.

## Working

After obfuscation, this email address:  example.one@example.org turns into 
this, but only for bots:

  var eWNnEZVIu = new Array('g','@example.or','example.one');
  document.getElementById('YvynIpoMZ').innerHTML =    
                 eWNnEZVIu[2]+eWNnEZVIu[1]+eWNnEZVIu[0];
  document.getElementById('YvynIpoMZ').href = 
  'mailto:'+eWNnEZVIu[2]+eWNnEZVIu[1]+eWNnEZVIu[0];
  Additional feature to handle non JS environment, can be turned off from 
  admin side.
  <noscript><img src="path to image" /></noscript>

## Requirements

Should work on any Drupal 7 installation. If non Js browser support is needed 
Gd library should be enabled, which will be on in most cases.

## Installation

 * git clone --branch 7.x-1.x abhiklpm@git.drupal.org:sandbox/abhiklpm/2465593.
 git spamfree_email

 * Select the field types you need to be linked: 
   admin/config/content/spamfree_email .

 * If support for non Javascript browser is enabled, GD library is a must 
 which will usually be installed on server.

## Usage

 * Install the module and go to configuration page and select the fields in 
 which obfuscation to be added.

 * For selecting the font family, you need to upload the font and click Save
  first and then only the font will appear in the font list.

## Known problems

This module currently replaces the text in a tags with corresponding email 
address.
eg: <a href="mailto:example@example.org">Click here</a>
will become:
<a href="mailto:example@example.org">example@example.org</a>

## To Do

Option to do the same on view fields

## Authors

Module developed by Abhilash RS (abhiklpm).
Module and project maintained by Abhilash RS.
