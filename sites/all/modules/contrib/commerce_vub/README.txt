-- INSTALLATION --

* Install as usual Drupal module, see http://drupal.org/node/895232 for further information.

-- CONFIGURATION --
1. This payment method can be configured as other payment methods. After enabling the module, "ePlatby VUB" should appear in the list of payment methods,
(it is page /admin/commerce/config/payment-methods)
2. After clicking on "ePlatby VUB" in the list, 'Editing reaction rule "ePlatby VUB"'  page appears
(page admin/commerce/config/payment-methods/manage/commerce_payment_vub)
VUB settings (like Merchant ID, Password etc.) can be edited in "Actions" table


-- HOW IT WORKS

When "ePlatby VUB" payment method is enabled, it can be chosen by users as payment method during checkout process.
When customer choose "ePlatby VUB", user is redirected offsite to third-party (VUB) website, where the payment will be processed. 
After payment is processed, user is redirected from VUB website back to eshop to continue (and finish) checkout process.
After return to eshop, this module automatically update status of payment transaction. There are three various scenarios, how the payment was processed:
Payment was processed succesfully - payment transaction status is set to "success"
Payment failed - payment transaction status is set to "failure"

Admistrators of eshop can check payment transaction status of particular orders on "Payment" tab on order page (admin/commerce/orders/[ORDER ID]/payment)




