COMMERCE STRIPE PAYMENT INTENT
---------------

Commerce Stripe Payment intent integrates Stripe with Drupal Commerce payment and checkout
system using Payment intents API. This module will fully integrate the Stripe to Drupal Commerce in
that way that clients can make payments straight in the shop in PCI-compliant
way without leaving the actual shop page. Stripe is a simple way to accept
payments online. With Stripe you can accept Visa, MasterCard, American Express,
Discover, JCB, and Diners Club cards directly on your store.


INSTALLING COMMERCE STRIPE PAYMENT INTENT MODULE
-------------------------------------

1. Download latest module from http://drupal.org/project/commerce_stripe_pi

2. Download Libraries module from http://drupal.org/project/libraries

3. Enable Commerce Stripe PI and Libraries modules as usual: /admin/modules

4. Download Stripe library from https://github.com/stripe/stripe-php and
   extract it to sites/all/libraries/stripe-php


CONFIGURING PAYMENT METHOD
--------------------------

1. Create an account at https://stripe.com/

2. Insert your API keys at the Stripe configuration page
   admin/commerce/config/payment-methods/manage/commerce_payment_commerce_stripe_pi
   Remember to test the functionality with the test keys before going live!


VARIABLES
---------
By default the advanced fraud detection is enabled, which means
stripe.js will be included on every page. Since this might have privacy
implications, thus you can disable it by setting
commerce_stripe_pi_advanced_fraud_enabled to FALSE. There is currently no UI
so you have to use drush:
drush vset commerce_stripe_pi_advanced_fraud_enabled FALSE

CREDITS
-------

Commerce Stripe PI integration is a fork and rework of commerce_stripe module and has been written by :

Fabien Clément - https://drupal.org/user/226961
