<?php

global $base_url;

$order = commerce_order_load($info['order_number']);

$payment_method_id = !empty($order->data['payment_method']) ? $order->data['payment_method'] : '';
$payment_method = $payment_method_id ? commerce_payment_method_instance_load($payment_method_id) : FALSE;
$payment_method_title = '';

if (is_array($payment_method) && !empty($payment_method['display_title'])) {
  $payment_method_title = $payment_method['display_title'];
}
elseif (!empty($order->data['stripe_pi'])) {
  $payment_method_title = t('Card (Stripe)');
}
else {
  $payment_method_title = t('Not specified');
}
$order_total = isset($order->commerce_order_total[LANGUAGE_NONE][0]['amount']) ? $order->commerce_order_total[LANGUAGE_NONE][0]['amount']/100 : '';
$currency = $order->commerce_order_total[LANGUAGE_NONE][0]['currency_code'] == 'EUR' ? '€' : '';
$payer_type = $order->field_payer_type[LANGUAGE_NONE][0]['value'];

$lid = $order->commerce_line_items[LANGUAGE_NONE][0]['line_item_id'];
$liobj = commerce_line_item_load($lid);
$prod_id = $liobj->commerce_product[LANGUAGE_NONE][0]['product_id'];

$show_order_table = TRUE;

global $user;

$default_lang = language_default()->language;

$accept_language = isset($_SERVER['HTTP_ACCEPT_LANGUAGE']) ? $_SERVER['HTTP_ACCEPT_LANGUAGE'] : '';

// Try language from payment metadata
if (!empty($order->data['stripe_pi']['payment_intent']['metadata']['language_mapped'])) {
  $target_lang = $order->data['stripe_pi']['payment_intent']['metadata']['language_mapped'];
}
// Use registered user's language if available
elseif ($user->uid && !empty($user->language)) {
  $target_lang = $user->language;
}
else if ($accept_language) {
	$target_lang = substr($accept_language, 0, 2);
}
// Fallback to site default
else {
  $target_lang = $default_lang;
}

?>

<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
    <style type="text/css" media="all">
		.field-name-commerce-order-total .commerce-price-formatted-components {
		  width: 33%;
		  margin-left: auto;
		}
		.field-name-commerce-order-total .commerce-price-formatted-components tr.component-type-commerce-price-formatted-amount {
		  font-weight: bold;
		}
		.products ul{
		  display:none;
		}
		tr.odd {
		  background: none repeat scroll 0 0 #FFFFFF;
		}
		tr.even {
		  background: none repeat scroll 0 0 #EFEFEF;
		}
		.component-title {
		  font-weight: bold;
		}

		.pmethod .label, .precipient .label, .ptotal .label {
			font-size: unset;
			color: unset;
		}

		table {
		  width:100%;
		  border-spacing: 0;
		  font-size: 12px;
		}
		table.details td {
		  padding: 0 5px;
		}
		table th {
		  background: #d2d2d2;
		  color: #000;
		  text-align: left;
		  padding: 0;
		  font-weight: bold;
		  border: 0;
		  text-transform: unset;
		}
		table th,
		.products table td {
		  padding: 4px 5px;
		}
		.component-total,
		.views-field-commerce-total {
		  width: 100px!important;
		  text-align: right;
		}
		.line-items {
		  border-bottom: 1px dotted #CCC;
		}
		.products table.commerce-price-formatted-components {
		  width: 200px;
		  padding-top: 20px;
		  float: right;
		}
		
		
		/* rovas styles */

		.logo-img {
			width: 25px;
		}

		.pmethod .label,
		.precipient .label,
		.ptotal .label {
			font-weight: bold;
		}

		.precipient .value, .ptotal .value  {
			font-weight: bold;
			font-size: 11pt;
		}
		.invoice-text {
			font-size: 12px;
			margin-bottom: 20px;
			display: block;
		}

		.products {
			margin-top: 20px;
		}

		.total {
			text-align: right;
			padding-right: 5px;
			font-weight: bold;
		}

		.footer-table {
			margin-top: 50px;
		}

		.thanks {
			text-align: center;
			font-size: 14pt;
			margin-top: 30px;
		}

		.paymethod, .rec-project, .ordr-total {
			padding-left: 10px;
		}
	</style>
  </head>
  <body>
    <table width="100%" border="0" cellspacing="0" cellpadding="1" align="center" bgcolor="#CCCCCC">
      <tr>
        <td>
          <table width="100%" border="0" cellspacing="0" cellpadding="5" align="center" bgcolor="#FFFFFF" style="font-family: verdana, arial, helvetica; font-size: 12px;">
            <tr>
              <td>
                <table width="100%" border="0" cellspacing="0" cellpadding="0" style="font-family: verdana, arial, helvetica; font-size: 11px;">
                  <tr>
                    <td nowrap="nowrap" style="line-height: 1.6em;" valign="middle">
											<!-- Invoice Header -->
											<img class="logo-img" src="<?php 
											$base_url = url('', array('absolute' => TRUE));
											print $base_url . 'sites/default/files/pictures/rovas_logo.png'; 
											?>" />
                    </td>
                  </tr>
                </table>
              </td>
            </tr>
            <tr>
              <td>
                <table width="100%" border="0" cellspacing="0" cellpadding="0" style="font-family: verdana, arial, helvetica; font-size: 12px;">
                  <tr>
                    <th colspan="2"><?php print t('Order Confirmation',[],['langcode' => $target_lang]); ?></th>
                  </tr>
                  <tr>
                    <td colspan="2">

                      <table class="details" width="100%" cellspacing="0" cellpadding="0" style="font-family: verdana, arial, helvetica; font-size: 1em;">
                        <tr>
                          <td valign="top" width="50%">
                            <br/>
                            <b><?php print t('Order Date:',[],['langcode' => $target_lang]); ?></b> <?php print date('j F, Y', $info['order_created']); ?><br/>
                            <br/>
                            <b><?php print t('Billing Address:',[],['langcode' => $target_lang]); ?></b><br />
                            <?php print isset($info['customer_billing']) ? $info['customer_billing'] : ''; ?><br />
                          </td>
                          <td valign="top" width="50%">
                            <br/>
                            <b><?php print t('Web Order No:',[],['langcode' => $target_lang]); ?></b> <?php print $info['order_number']; ?><br/>
                            <br/>
                            <b><?php print t('Email Address:',[],['langcode' => $target_lang]); ?></b> <?php print $info['order_mail']; ?><br/>
                            <?php if (isset($info['customer_shipping']) && !empty($info['customer_shipping'])): ?>
                            <br/>
                            <b><?php print t('Shipping Address:',[],['langcode' => $target_lang]); ?></b><br />
                            <?php print $info['customer_shipping']; ?><br />
                            <?php endif; ?>
                          </td>
                        </tr>
                      </table>

                    </td>
                  </tr>
                </table>
              </td>
            </tr>

            <tr>
              <td class="paymethod"><?php 
                      
				print '<div class="pmethod">';
				print '<span class="label">';
				print t('Payment method',[],['langcode' => $target_lang]) . ': ';
				print '</span>';
				print '<span class="value">';
				print check_plain($payment_method_title);
				print '</span>';
				print '</div>';

			  ?></td>
            </tr>

		<?php 
		
		$recipientID = $order->field_payment_recipient[LANGUAGE_NONE][0]['value'];
		
		if ($payer_type == 'user') {
			$recipient = entity_metadata_wrapper('user', $recipientID);
			$plainURL = 'user/' . $recipientID;
			$rec_url = $base_url . '/' . $plainURL;
			$rec_name = $recipient->field_full_name->value() . ' (' . $recipient->label() . ')';
			$recipient_label = t('Recipient',[],['langcode' => $target_lang]);
		}
		else {
			$recipient = entity_metadata_wrapper('node', $recipientID);
			$plainURL = 'node/' . $recipientID;
			$rec_url = $base_url . drupal_get_path_alias($plainURL);
			$rec_name = $recipient->label();
			$recipient_label = t('Payment recipient',[],['langcode' => $target_lang]);
		}
		
		?>

		<?php if (isset($order->field_payment_recipient[LANGUAGE_NONE][0]['value'])): ?>
            <tr>
              <td class="rec-project">
			<?php
				print '<div class="precipient">';
				print '<span class="label">';
				print $recipient_label . ': ';
				print '</span>';
				print '<span class="value">';
				print '<a href="'.$rec_url.'">';
				print $rec_name;
				print '</a>';
				print '</span>';
				print '</div>';
			?>
			  </td>
            </tr>
			<?php
				//user directly rewarded a project (did not buy shares)
				if ((int)$prod_id === 3): ?>
				<tr>
				  <td class="ordr-total">
				<?php
					print '<div class="ptotal">';
					print '<span class="label">';
					print t('Order total',[],['langcode' => $target_lang]) . ': ';
					print '</span>';
					print '<span class="value">';
					print number_format($order_total, 2, ',', ' ') . ' ' . $currency;
					print '</span>';
					print '</div>';
					
					$show_order_table = FALSE;
				  ?>
				  </td>
				</tr>
			<?php endif; ?>
		<?php endif; ?>

		<?php if ($show_order_table): ?>
            <tr>
              <td>
                <table class="products" width="100%" border="0" cellspacing="0" cellpadding="0" align="center" style="font-family: verdana, arial, helvetica; font-size: 12px;">
                  <tbody>
                    <tr>
                      <td class="line-items"><?php print isset($info['line_items']) ? $info['line_items'] : ''; ?></td>
                    </tr>
                    <tr>
                      <td class="total" ><?php 
                      
                      print $order_total > 0 ? t('Order total',[],['langcode' => $target_lang]) . ': ' . number_format($order_total, 2, ',', ' ') . ' ' . $currency : ''; 
                      
                      ?></td>
                    </tr>
                  </tbody>
                </table>
              </td>
            </tr>
		<?php endif; ?>

	<?php
	$bank_info_html = rovas_project_reward_render_bank_transfer_info($order, $target_lang);
	if (!empty($bank_info_html)): ?>
            <tr>
              <td><?php print $bank_info_html; ?></td>
            </tr>
	<?php endif; ?>

            <tr>
              <td>
				<?php
					print '<div class="thanks">';
					print t('Thank you for your order',[],['langcode' => $target_lang]);
					print '</div>';
				?>
              </td>
            </tr>

            <tr>
              <td>
                <table class="footer-table">
                  <tr>
                    <td colspan="2" style="background: #EEEEEE; color: #666666; padding: 1em; font-size: 0.9em; line-height: 1.6em; border-top: #CCCCCC 1px dotted; text-align: center;">
                      <!-- Invoice Footer -->
                      <?php
                      	print t('<strong><a href="'.url('<front>', array('absolute' => TRUE)).'" target="_blank">Rovas</a></strong> - enabling participation in the <a href="https://neofund.sk/">NEO economy</a>',[],['langcode' => $target_lang]);
						print '<br>';

						$site_mail = variable_get('site_mail', NULL);
						$subject = t('Rovas order number',[],['langcode' => $target_lang]) . ' ' . $info['order_number'];
						print t('Service provided by <a href="@link"><strong>NeoFund s.r.o.</strong></a>',['@link' => 'https://neofund.sk'],['langcode' => $target_lang]) . ' • ';
						print '<a href="mailto:'.$site_mail.'?subject='.$subject.'">' . t('Contact us',[],['langcode' => $target_lang]) . '</a>';
					  ?>
                    </td>
                  </tr>
                </table>
              </td>
            </tr>
          </table>
        </td>
      </tr>
    </table>
  </body>
</html>
