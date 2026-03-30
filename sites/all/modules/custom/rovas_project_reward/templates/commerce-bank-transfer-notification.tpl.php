<?php
/**
 * @file
 * Default template for bank transfer reconciliation notification emails.
 *
 * Available variables:
 *   - $mail_key:    The reconciliation outcome key (see below).
 *   - $content:     Pre-built HTML string with the outcome-specific body paragraphs.
 *   - $order_id:    Commerce order ID (used in footer contact link subject).
 *   - $target_lang: Language code used for translations.
 *   - $base_url:    Absolute site base URL.
 *   - $site_mail:   Site contact email address.
 *
 * $mail_key values and their meanings:
 *   bank_transfer_confirmed  – payment received and confirmed; order completed.
 *   bank_transfer_mismatch   – amount received differs from expected; manual review.
 *   bank_transfer_failed     – payment rejected or failed.
 *   bank_transfer_expired    – payment window expired without a matching transfer.
 *
 * The rovas theme overrides this template with its own copy at:
 *   sites/all/themes/rovas/commerce-bank-transfer-notification.tpl.php
 */

$logo_url = $base_url . '/sites/default/files/pictures/rovas_logo.png';
$subject_encoded = rawurlencode(t('Rovas order number', array(), array('langcode' => $target_lang)) . ' ' . $order_id);
?>
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
  <style type="text/css" media="all">
    body { font-family: verdana, arial, helvetica; font-size: 12px; }
    table { width: 100%; border-spacing: 0; font-size: 12px; }
    .notification-inner { background: #ffffff; padding: 20px; font-family: verdana, arial, helvetica; font-size: 12px; }
    .notification-header { border-bottom: 1px solid #cccccc; padding-bottom: 10px; margin-bottom: 15px; }
    .notification-header img { width: 25px; vertical-align: middle; }
    .notification-body p { line-height: 1.6em; margin: 0 0 0.8em 0; }
    .notification-footer { margin-top: 40px; background: #eeeeee; color: #666666; padding: 1em; font-size: 0.9em; line-height: 1.6em; border-top: 1px dotted #cccccc; text-align: center; }
  </style>
</head>
<body>
<table width="100%" border="0" cellspacing="0" cellpadding="1" bgcolor="#CCCCCC">
  <tr>
    <td>
      <table width="100%" border="0" cellspacing="0" cellpadding="5" bgcolor="#FFFFFF" class="notification-inner">
        <tr>
          <td>
            <div class="notification-header">
              <img src="<?php print $logo_url; ?>" alt="Rovas" />
            </div>
            <div class="notification-body">
              <?php print $content; ?>
            </div>
            <div class="notification-footer">
              <?php
                print t('<strong><a href="@front" target="_blank">Rovas</a></strong> – enabling participation in the <a href="https://neofund.sk/">NEO economy</a>',
                  array('@front' => url('<front>', array('absolute' => TRUE))),
                  array('langcode' => $target_lang));
                print '<br />';
                print t('Service provided by <a href="@link"><strong>NeoFund s.r.o.</strong></a>',
                  array('@link' => 'https://neofund.sk'),
                  array('langcode' => $target_lang));
                print ' &bull; ';
                print '<a href="mailto:' . check_plain($site_mail) . '?subject=' . $subject_encoded . '">'
                  . t('Contact us', array(), array('langcode' => $target_lang)) . '</a>';
              ?>
            </div>
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>
</body>
</html>
