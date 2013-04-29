<?php
/**
 * @file
 * Infusionsoft integration with conditional actions.
 */

/**
 * Conditional Actions for Ubercart
 *
 * Implements hook_ca_action().
 */
function infusionsoft_ca_action() {
  $order_arg = array(
    '#entity' => 'uc_order',
    '#title' => t('Order'),
  );
  $actions['infusionsoft_tags'] = array(
    '#title' => t('Add/Remove a Tag to a user in infusionsoft'),
    '#category' => t('Infusionsoft'),
    '#callback' => 'infusionsoft_action_tag',
    '#arguments' => array(
      'account' => $order_arg,
    ),
  );
  $actions['infusionsoft_campaign'] = array(
    '#title' => t('Add/Remove/Pause/Resume Follow up sequence for a user in infusionsoft'),
    '#category' => t('Infusionsoft'),
    '#callback' => 'infusionsoft_action_campaign',
    '#arguments' => array(
      'account' => $order_arg,
    ),
  );
  $actions['infusionsoft_run_action_set'] = array(
    '#title' => t('Run Action Set on a user in infusionsoft.'),
    '#category' => t('Infusionsoft'),
    '#callback' => 'infusionsoft_action_run_action_set',
    '#arguments' => array(
      'account' => $order_arg,
    ),
  );
  $actions['infusionsoft_add_to_database'] = array(
    '#title' => t('Add/Update a User in the Infusionsoft database.'),
    '#category' => t('Infusionsoft'),
    '#callback' => 'infusionsoft_action_add_to_database',
    '#arguments' => array(
      'account' => $order_arg,
    ),
  );
  return $actions;
}

function infusionsoft_ca_condition() {
  $order_arg = array(
    '#entity' => 'uc_order',
    '#title' => t('Order'),
  );
  $conditions = array(
    'infusionsoft_user_has_tag_condition' => array(
      '#title' => t('Check an order\'s user has a specific infusionsoft tag'),
      '#category' => t('Infusionsoft'),
      '#callback' => 'infusionsoft_condition_user_has_tag',
      '#arguements' => array(
        'account' => $order_arg,
      ),
    ),
  );
  return $conditions;
}

/**
 * Add Infusionsoft Tag
 *
 * Conditional action callback.
 */

function infusionsoft_action_tag(&$order, $settings) {
  // get the uid from the object
  if (isset($order->uid)) {
    $user = user_load($order->uid);
  }
  else {
    global $user;
  }
  // not anonymous
  if ($user->uid > 0) {
    // tag_user
    $contact_id = infusionsoft_contactid_by_user($user);
    switch ($settings['action']) {
      case 'add':
        $result = infusionsoft_send_request('ContactService.addToGroup', $contact_id, $settings['tag']);
        // record a message noting the action taken
        _infusionsoft_devel('conditional action', 'Added contact Group %contact_group_id to %name in infusionsoft / uid=%uid.', array('%contact_group_id' => $settings['contact_group'], '%name' => check_plain($user->name), '%uid' => $user->uid));
        break;
      case 'remove':
        $result = infusionsoft_send_request('ContactService.removeFromGroup', $contact_id, $settings['tag']);
        // record a message noting the action taken
        _infusionsoft_devel('conditional action', 'Removed contact Group %contact_group_id from %name in infusionsoft / uid=%uid.', array('%contact_group_id' => $settings['contact_group'], '%name' => check_plain($user->name), '%uid' => $user->uid));
        break;
    }
  }
}

function infusionsoft_action_tag_form($form_state, $settings = array(), $arguments = array()) {
  $form['action'] = array(
    '#title' => t('Action'),
    '#type' => 'select',
    '#description' => t('Add or remove a tag from a user in infusionsoft'),
    '#options' => array(
      'add' => 'add',
      'remove' => 'remove',
    ),
    '#default_value' => isset($settings['action']) ? $settings['action'] : '',
  );
  $form['tag'] = array(
    '#title' => t('Tags'),
    '#type' => 'select',
    '#description' => t('Choose a tag you would like to add to a user'),
    '#options' => _infusionsoft_get_contact_group_select(),
    '#default_value' => isset($settings['tag']) ? $settings['tag'] : '',
  );
  $form['#validate'][] = 'infusionsoft_action_tag_validate';
  $form['#submit'][] = 'infusionsoft_action_tag_submit';
  return $form;
}

function infusionsoft_action_tag_validate($form, $form_state) {
  $numeric = is_numeric($form_state['tag']);
  if (!$numeric) form_set_error('tag', 'invalid option');
  switch ($form_state['action']) {
    case 'add':
    case 'remove':
      break;
    default:
      form_set_error('action', 'invalid option');
    }
}

function infusionsoft_action_tag_submit($form, $form_state, $arguements) {
  return array('tag' => $form_state['tag'], 'action' => $form_state['action']);
}

/**
 * Submit callback for infusionsoft_action_remove_tag_form().
 */
function infusionsoft_action_remove_tag_submit($form, $form_state) {
  return array('contact_group' => $form_state['values']['tag']);
}

/**
 * Infusionsoft Campaign User Actions
 *
 * Conditional action callback.
 */
function infusionsoft_action_campaign(&$order, $settings) {
  // get the uid from the object
  if (isset($order->uid)) {
    $user = user_load($order->uid);
  }
  else {
    global $user;
  }
  // not anonymous
  if ($user->uid > 0) {
    // assign campaign
    $contact_id = infusionsoft_contactid_by_user($user);
    switch ($settings['action']) {
      case 'add':
        $result = infusionsoft_campaign_update($contact_id, $settings['campaign'], 'assign');
        // record a message noting the action taken
        if ($result) {
          _infusionsoft_devel('conditional action', 'Added follow up sequence %followup_id to %name in infusionsoft / uid=%uid.', array('%followup_id' => $settings['campaign'], '%name' => check_plain($user->name), '%uid' => $user->uid));
        }
        else {
          _infusionsoft_devel('conditional action', 'Unable to add campaign %followup_id to %name in infusionsoft / uid=%uid.', array('%followup_id' => $settings['campaign'], '%name' => check_plain($user->name), '%uid' => $user->uid));
        }
        break;
      case 'remove':
        $result = infusionsoft_campaign_update($contact_id, $settings['campaign'], 'remove');
        // record a message noting the action taken
        if ($result) {
          _infusionsoft_devel('conditional action', 'Removed follow up sequence %followup_id from %name in infusionsoft / uid=%uid.', array('%followup_id' => $settings['campaign'], '%name' => check_plain($user->name), '%uid' => $user->uid));
        }
        break;
      case 'pause':
        $result = infusionsoft_campaign_update($contact_id, $settings['campaign'], 'pause');
        // record a message noting the action taken
        if ($result) {
          _infusionsoft_devel('conditional action', 'Paused follow up sequence %followup_id for %name in infusionsoft / uid=%uid.', array('%followup_id' => $settings['campaign'], '%name' => check_plain($user->name), '%uid' => $user->uid));
        }
        break;
      case 'resume':
        $result = infusionsoft_campaign_update($contact_id, $settings['campaign'], 'resume');
        // record a message noting the action taken
        if ($result) {
          _infusionsoft_devel('conditional action', 'Resumed follow up sequence %followup_id for %name in infusionsoft / uid=%uid.', array('%followup_id' => $settings['campaign'], '%name' => check_plain($user->name), '%uid' => $user->uid));
        }
        break;
    }
  }
}


/**
 * Infusionsoft Campaign User Actions
 *
 *  Implementation of Drupal action form.
 *  Reference: http://www.sysarchitects.com/node/47
 *
 * A drupal form which allows you to select an infusionsoft Campaign
 *
 */
function infusionsoft_action_campaign_form($form_state, $settings = array(), $arguments = array()) {
  $form['action'] = array(
    '#title' => t('Action'),
    '#type' => 'select',
    '#description' => t('Add, remove, pause, or resume a follow up sequence for a user in infusionsoft'),
    '#options' => array(
      'add' => 'add to follow up sequence',
      'remove' => 'remove from follow up sequence',
      'pause' => 'pause follow up sequence',
      'resume' => 'resume follow up sequence',
    ),
    '#default_value' => isset($settings['action']) ? $settings['action'] : '',
  );
  $form['campaign'] = array(
    '#title' => t('Follow up sequence'),
    '#type' => 'select',
    '#description' => t('Select an infusionsoft follow up sequence you would like to add to a user'),
    '#options' => _infusionsoft_get_follow_up_select(),
    '#default_value' => isset($settings['campaign']) ? $settings['campaign'] : '',
  );
  $form['#validate'][] = 'infusionsoft_action_add_campaign_validate';
  $form['#submit'][] = 'infusionsoft_action_add_campaign_submit';
  return $form;
}

function infusionsoft_action_campaign_validate($form, $form_state) {
  $numeric = is_numeric($form_state['campaign']);
  if (!$numeric) form_set_error('campaign', 'invalid option');
  switch ($form_state['action']) {
    case 'add':
    case 'remove':
    case 'pause':
    case 'resume':
      break;
    default:
      form_set_error('action', 'invalid option');
    }
}

function infusionsoft_action_campaign_submit($form, $form_state) {
  return array('campaign' => $form_state['campaign'], 'action' => $form_state['action']);
}

/**
 * Run Infusionsoft ActionSet on User
 *
 * Implementation conditional action callback.
 */
function infusionsoft_action_run_action_set(&$order, $settings) {
  // get the uid from the object
  if (isset($order->uid)) {
    $user = user_load($order->uid);
  }
  else {
    global $user;
  }
  // not anonymous
  if ($user->uid > 0) {
    // assign campaign
    $contact_id = infusionsoft_contactid_by_user($user);
    $result = infusionsoft_run_actionset($contact_id, $settings['action_set']);
    // record a message noting the action taken
    if ($result) {
      _infusionsoft_devel('conditional action', 'Ran Infusionsoft action set %action_set_id from %name in infusionsoft / uid=%uid.', array('%action_set_id' => $settings['campaign'], '%name' => check_plain($user->name), '%uid' => $user->uid));
    }
  }
}

/**
 * Run Infusionsoft ActionSet on User
 *
 *  Implementation of Drupal action form.
 *  Reference: http://www.sysarchitects.com/node/47
 *
 * A drupal form which allows you to select an infusionsoft Campaign
 *
 */
function infusionsoft_action_run_action_set_form($form_state, $settings = array(), $arguments = array()) {
  $form['action_set'] = array(
    '#title' => t('Action Set'),
    '#type' => 'select',
    '#description' => t('Select an infusionsoft action set you would like to run for a user'),
    '#options' => _infusionsoft_get_action_set_select(),
    '#default_value' => isset($settings['action_set']) ? $settings['action_set'] : '',
  );
  $form['#validate'][] = 'infusionsoft_action_run_action_set_validate';
  $form['#submit'][] = 'infusionsoft_action_run_action_set_submit';
  return $form;
}

function infusionsoft_action_run_action_set_validate($form, $form_state) {
  $numeric = is_numeric($form_state['action_set']);
  if (!$numeric) {
    form_set_error('action_set', 'invalid option');
  }
}

function infusionsoft_action_run_action_set_submit($form, $form_state) {
  return array('action_set' => $form_state['values']['action_set']);
}

/**
 * Add to Infusionsoft Contact Database
 *
 * Implementation conditional action callback.
 */
function infusionsoft_action_add_to_database(&$order, $settings) {
  // get the uid from the object
  if (isset($order->uid)) {
    $user = user_load($order->uid);
  }
  else {
    global $user;
  }
  // Not anonymous
  if ($user->uid > 0) {

    // Default to default mapping so that existing users installs don't stop working.
    if (empty($settings['map_name'])) {
      $settings['map_name'] = 'default';
    }

    // Retrieve contact data.
    $contact_data = infusionsoft_contact_get_map($user, $settings['map_name']);

    if (!empty($contact_data)) {
      // update user in infusionsoft.
      $contact_id = infusionsoft_update_drupal_user($contact_data, $user);
    }
    // record a message noting the action taken
    _infusionsoft_devel('conditional action', 'Add %name to infusionsoft database / uid=%uid.', array('%name' => check_plain($user->name), '%uid' => $user->uid));
  }
}

/**
 * Add new user to database
 *
 *  Implementation of Drupal action form.
 *  Reference: http://www.sysarchitects.com/node/47
 *
 * A drupal form which allows you to select a database mapping.
 *
 */
function infusionsoft_action_add_to_database_form($form_state, $settings = array(), $arguments = array()) {
  $form['map'] = array(
    '#title' => t('Contact mapping'),
    '#type' => 'select',
    '#description' => t('Select an infusionsoft contact mapping to send data to your user with.'),
    '#options' => _infusionsoft_contact_map_select(),
    '#default_value' => isset($settings['map_name']) ? $settings['map_name'] : 'default',
  );
  $form['#validate'][] = 'infusionsoft_action_add_to_database_validate';
  $form['#submit'][] = 'infusionsoft_action_add_to_database_submit';
  return $form;
}

function infusionsoft_action_add_to_database_validate($form, $form_state) {
  $map_info = infusionsoft_contact_maps();

  if (empty($map_info[$form_state['map']])) {
    form_set_error('map', 'invalid map');
  }
}

function infusionsoft_action_add_to_database_submit($form, $form_state) {
  return array('map_name' => $form_state['values']['map']);
}

/**
 * Condition: User has a tag
 *
 *  Implementation of conditional action condition callback.
 */
function infusionsoft_condition_user_has_tag($order, $settings) {
  if ($order->uid > 0) {
    $user = user_load($order->uid);
    $contact_id = infusionsoft_contactid_by_user($user);
    $query = array(
      'UserId' => $contact_id,
      'GroupId' => $settings['tag'],
    );
    $return_fields = array('UserId');
    $result = infusionsoft_send_request('DataService.query', 'GroupAssign', 1, 0, $query, $return_fields);
    if (empty($result) != TRUE) {
      return TRUE;
    }
  }
  return FALSE;
}

function infusionsoft_condition_user_has_tag_form($form_state, $settings = array(), $arguments = array()) {
  $form['tag'] = array(
    '#title' => t('Tags'),
    '#type' => 'select',
    '#description' => t('Choose a tag you would like to add to a user'),
    '#options' => _infusionsoft_get_contact_group_select(),
    '#default_value' => isset($settings['tag']) ? $settings['tag'] : '',
  );
  $form['#validate'][] = 'infusionsoft_condition_user_has_tag_validate';
  $form['#submit'][] = 'infusionsoft_condition_user_has_tag_submit';
  return $form;
}

function infusionsoft_condition_user_has_tag_validate($form, $form_state) {
  $numeric = is_numeric($form_state['tag']);
  if (!$numeric) {
    form_set_error('tag', 'invalid option');
  }
}

function infusionsoft_condition_user_has_tag_submit($form, $form_state) {
  return array('tag' => $form_state['tag']);
}
