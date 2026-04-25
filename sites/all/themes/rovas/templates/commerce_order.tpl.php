<?php
	$content = $element->content;
?>

<div class="<?php print $classes; ?>">
  <?php
		print render($content['commerce_customer_billing']);
	  print render($content['commerce_line_items']);
		print render($content['commerce_order_total']);
  ?>

	<?php if (!empty($element->data['payment_method']) && $element->data['payment_method'] == 'bank_transfer|commerce_payment_bank_transfer'): ?>
		<div class="payment-info">
			<div class="payment-info-label"><?php print t('Payment information:'); ?></div>
		<?php
				$payment_method = commerce_payment_method_instance_load($element->data['payment_method']);
				print commerce_bank_transfer_bank_details($element->order_id, $payment_method); 
		?>
		</div>
	<?php endif; ?>
</div>
