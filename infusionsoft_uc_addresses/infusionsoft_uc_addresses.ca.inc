<?php
/**
 * @file
 *  Conditional actions which will send uc_addresses info to infusionsoft.
 */


/**
 * Implements hook_ca_action().
 */
function infusionsoft_uc_addresses_ca_action() {
  $order_arg = array(
    '#entity' => 'uc_order',
    '#title' => t('Order'),
  );
  $actions['infusionsoft_uc_address_add_to_database'] = array(
    '#title' => t('DEPRECIATED: Add uc_addresses information to a user in the infusionsoft database'),
    '#category' => t('Infusionsoft'),
    '#callback' => 'infusionsoft_uc_addresses_action_add_to_database',
    '#arguments' => array(
      'account' => $order_arg,
    ),
  );
  return $actions;
}

/**
 * Add to Infusionsoft Contact Database
 *
 * Implements conditional action callback.
 */
function infusionsoft_uc_addresses_action_add_to_database(&$order, $settings) {
  // Get the uid from the object.
  if (isset($order->uid)) {
    $user = user_load($order->uid);
  }
  else {
    global $user;
  }
  infusionsoft_uc_addresses_send($user);
}
