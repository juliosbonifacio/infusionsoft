<?php
/**
 * @file
 * Provides integration with the drupal core actions module
 */

/**
 * Actions
 *
 * Implements hook_action_info().
 *
 * See description here http://drupal.org/node/172152
 */
function infusionsoft_action_info() {
  return array(
    'infusionsoft_add_tag_to_user_action' => array(
      'description' => t('Infusionsoft: Add tag to user'),
      'type' => 'user',
      'configurable' => TRUE,
      'hooks' => array(
        'any' => TRUE
      ),
    ),
    'infusionsoft_remove_tag_from_user_action' => array(
      'description' => t('Infusionsoft: Remove tag from user'),
      'type' => 'user',
      'configurable' => TRUE,
      'hooks' => array(
        'any' => TRUE
      ),
    ),
    'infusionsoft_add_user_to_database_action' => array(
      'description' => t('Infusionsoft: Add/update user to the Infusionsoft database'),
      'type' => 'user',
      'configurable' => TRUE,
      'hooks' => array(
        'any' => FALSE
      ),
    ),
    'infusionsoft_add_campaign_to_user_action' => array(
      'description' => t('Infusionsoft: Add campaign to user'),
      'type' => 'user',
      'configurable' => TRUE,
      'hooks' => array(
        'any' => FALSE
      ),
    ),
    'infusionsoft_remove_campaign_from_user_action' => array(
      'description' => t('Infusionsoft: Remove campaign from user'),
      'type' => 'user',
      'configurable' => TRUE,
      'hooks' => array(
        'any' => FALSE
      ),
    ),
    'infusionsoft_pause_campaign_for_user_action' => array(
      'description' => t('Infusionsoft: Pause campaign for user'),
      'type' => 'user',
      'configurable' => TRUE,
      'hooks' => array(
        'any' => FALSE
      ),
    ),
    'infusionsoft_resume_campaign_for_user_action' => array(
      'description' => t('Infusionsoft: Resume campaign for user'),
      'type' => 'user',
      'configurable' => TRUE,
      'hooks' => array(
        'any' => FALSE
      ),
    ),
    'infusionsoft_run_action_set_action' => array(
      'description' => t('Infusionsoft: Run action set on user'),
      'type' => 'user',
      'configurable' => TRUE,
      'hooks' => array(
        'any' => FALSE
      ),
    ),
  );
}

/**
 * Add Infusionsoft Tag
 *
 * Implementation of a Drupal action.
 * Adds an infusionsoft contact group to the current user.
 *
 * Note: Groups are now called tags in infusionsoft.
 */
function infusionsoft_add_tag_to_user_action(&$object, $context = array()) {
  // Get the user object.
  if (isset($object->uid)) {
    $user = user_load($object->uid);
  }
  elseif (isset($context['uid'])) {
    $user = user_load($context['uid']);
  }
  else {
    global $user;
  }
  // Make sure we have a user record.
  if (!empty($user->uid)) {
    // Tag user.
    $contact_id = infusionsoft_contactid_by_user($user);
    $result = infusionsoft_send_request('ContactService.addToGroup', $contact_id, $context['contact_group']);
    // record a message noting the action taken
    _infusionsoft_devel('action', 'Added tag %contact_group_id to %name in infusionsoft / uid=%uid.', array('%contact_group_id' => $context['contact_group'], '%name' => check_plain($user->name), '%uid' => $user->uid));
  }
}

/**
 * Add Infusionsoft Tag
 *
 *  Implementation of Drupal action form.
 *  Reference: http://www.sysarchitects.com/node/47
 *
 * A drupal form which allows you to select an infusionsoft Contact Group
 *
 */
function infusionsoft_add_tag_to_user_action_form($context) {
  $form['tag'] = array(
    '#title' => t('Tags'),
    '#type' => 'select',
    '#description' => t('Select a tag you would like to add to a user'),
    '#options' => _infusionsoft_get_contact_group_select(),
    '#default_value' => isset($context['contact_group']) ? $context['contact_group'] : '',
  );
  $form['#validate'][] = 'infusionsoft_add_tag_to_user_action_validate';
  $form['#submit'][] = 'infusionsoft_add_tag_to_user_action_submit';
  return $form;
}

function infusionsoft_add_tag_to_user_action_validate($form, $form_state) {
  $numeric = is_numeric($form_state['values']['tag']);
  if (!$numeric) {
    form_set_error('tag', 'invalid option');
  }
}

function infusionsoft_add_tag_to_user_action_submit($form, $form_state) {
  return array('contact_group' => $form_state['values']['tag']);
}

/**
 * Remove Infusionsoft Tag
 *
 * Implementation of a Drupal action.
 * Removes an infusionsoft contact group from the current user.
 *
 */
function infusionsoft_remove_tag_from_user_action(&$object, $context = array()) {
  // Get the user object.
  if (isset($object->uid)) {
    $user = user_load($object->uid);
  }
  elseif (isset($context['uid'])) {
    $user = user_load($context['uid']);
  }
  else {
    global $user;
  }
  // Make sure we have a user record.
  if (!empty($user->uid)) {
    // tag_user
    $contact_id = infusionsoft_contactid_by_user($user);
    $result = infusionsoft_send_request('ContactService.removeFromGroup', $contact_id, $context['contact_group']);
    // record a message noting the action taken
    _infusionsoft_devel('action', 'Removed tag %contact_group_id from %name in infusionsoft / uid=%uid.', array('%contact_group_id' => $context['contact_group'], '%name' => check_plain($user->name), '%uid' => $user->uid));
  }
}

/**
 * Remove Infusionsoft Tag
 *
 *  Implementation of Drupal action form.
 *  Reference: http://www.sysarchitects.com/node/47
 *
 * Drupal form which allows you to select an infusionsoft Contact Group
 *
 */
function infusionsoft_remove_tag_from_user_action_form($context) {
  $form['tag'] = array(
    '#title' => t('Tags'),
    '#type' => 'select',
    '#description' => t('Choose a tag you would like to remove from a user'),
    '#options' => _infusionsoft_get_contact_group_select(),
    '#default_value' => isset($context['contact_group']) ? $context['contact_group'] : '',
  );
  $form['#validate'][] = 'infusionsoft_remove_tag_from_user_action_validate';
  $form['#submit'][] = 'infusionsoft_remove_tag_from_user_action_submit';
  return $form;
}

function infusionsoft_remove_tag_from_user_action_validate($form, $form_state) {
  $numeric = is_numeric($form_state['values']['tag']);
  if (!$numeric) {
    form_set_error('tag', 'invalid option');
  }
}

function infusionsoft_remove_tag_from_user_action_submit($form, $form_state) {
  return array('contact_group' => $form_state['values']['tag']);
}

/**
 * Add to Infusionsoft Contact Database
 *
 * Implementation of a Drupal action.
 * Adds the current user to the infusionsoft database.
 *
 */
function infusionsoft_add_user_to_database_action(&$object, $context = array()) {
  // Get the user object.
  if (isset($object->uid)) {
    $user = user_load($object->uid);
  }
  elseif (isset($context['uid'])) {
    $user = user_load($context['uid']);
  }
  else {
    global $user;
  }
  // Make sure we have a user record.
  if (!empty($user->uid)) {

    // Set the default map.
    if (empty($context['map_name'])) {
      $context['map_name'] = 'default';
    }
    $contact_data = infusionsoft_contact_get_map($user, $context['map_name']);
    // Don't update if there is no contact data.
    if (!empty($contact_data)) {
      // update user in infusionsoft.
      $contact_id = infusionsoft_update_drupal_user($contact_data, $user);
    }
    // record a message noting the action taken
    _infusionsoft_devel('action', "Add/update %name to infusionsoft database / uid=%uid.<br />map name: %map_name<br />mapped data: <pre>%mapped_data</pre>", array('%contact_group_id' => $context['contact_group'], '%name' => check_plain($user->name), '%uid' => $user->uid, '%map_name' => $context['map_name'], '%mapped_data' => print_r($contact_data, TRUE)));
  }
}

/**
 * Add/update user to the infusionsoft database form.
 *
 *  Implementation of Drupal action form.
 *  Reference: http://www.sysarchitects.com/node/47
 *
 * Drupal form which allows you to select an infusionsoft Contact Group
 *
 */
function infusionsoft_add_user_to_database_action_form($context) {
  $form['map_name'] = array(
    '#title' => t('Contact mapping'),
    '#type' => 'select',
    '#description' => t('Select an infusionsoft contact mapping to send data to your user with.'),
    '#options' => _infusionsoft_contact_map_select(),
    '#default_value' => isset($context['map_name']) ? $context['map_name'] : 'default',
  );

  $form['#validate'][] = 'infusionsoft_add_user_to_database_action_validate';
  $form['#submit'][] = 'infusionsoft_add_user_to_database_action_submit';
  return $form;
}

function infusionsoft_add_user_to_database_action_validate($form, $form_state) {
  $map_info = infusionsoft_contact_maps();

  if (empty($map_info[$form_state['values']['map_name']])) {
    form_set_error('map_name', 'invalid map');
  }
}

function infusionsoft_add_user_to_database_action_submit($form, $form_state) {
  return array('map_name' => $form_state['values']['map_name']);
}

/**
 * Add Infusionsoft Campaign to User
 *
 * Implementation of a Drupal action.
 * Adds an infusionsoft contact group to the current user.
 *
 */
function infusionsoft_add_campaign_to_user_action(&$object, $context = array()) {
  // Get the user object.
  if (isset($object->uid)) {
    $user = user_load($object->uid);
  }
  elseif (isset($context['uid'])) {
    $user = user_load($context['uid']);
  }
  else {
    global $user;
  }
  // Make sure we have a user record.
  if (!empty($user->uid)) {
    // assign campaign
    $contact_id = infusionsoft_contactid_by_user($user);
    $result = infusionsoft_campaign_update($contact_id, $context['campaign'], 'assign');
    // record a message noting the action taken
    if ($result) {
      _infusionsoft_devel('action', 'Added follow up sequence %followup_id to %name in infusionsoft / uid=%uid.', array('%followup_id' => $context['campaign'], '%name' => check_plain($user->name), '%uid' => $user->uid));
    }
  }
}


/**
 * Add Infusionsoft Campaign to User
 *
 *  Implementation of Drupal action form.
 *  Reference: http://www.sysarchitects.com/node/47
 *
 * A drupal form which allows you to select an infusionsoft Campaign
 *
 */
function infusionsoft_add_campaign_to_user_action_form($context) {
  $form['campaign'] = array(
    '#title' => t('Follow up sequence'),
    '#type' => 'select',
    '#description' => t('Choose an infusionsoft follow up sequence you would like to add to a user'),
    '#options' => _infusionsoft_get_follow_up_select(),
    '#default_value' => isset($context['campaign']) ? $context['campaign'] : '',
  );
  $form['#validate'][] = 'infusionsoft_add_campaign_to_user_action_validate';
  $form['#submit'][] = 'infusionsoft_add_campaign_to_user_action_submit';
  return $form;
}

function infusionsoft_add_campaign_to_user_action_validate($form, $form_state) {
  $numeric = is_numeric($form_state['values']['campaign']);
  if (!$numeric) {
    form_set_error('campaign', 'invalid option');
  }
}


function infusionsoft_add_campaign_to_user_action_submit($form, $form_state) {
  return array('campaign' => $form_state['values']['campaign']);
}

/**
 * Remove Infusionsoft Campaign from User
 *
 * Implementation of a Drupal action.
 * Adds an infusionsoft contact group to the current user.
 *
 */
function infusionsoft_remove_campaign_from_user_action(&$object, $context = array()) {
  // Get the user object.
  if (isset($object->uid)) {
    $user = user_load($object->uid);
  }
  elseif (isset($context['uid'])) {
    $user = user_load($context['uid']);
  }
  else {
    global $user;
  }
  // Make sure we have a user record.
  if (!empty($user->uid)) {
    // assign campaign
    $contact_id = infusionsoft_contactid_by_user($user);
    $result = infusionsoft_campaign_update($contact_id, $context['campaign'], 'remove');
    // record a message noting the action taken
    if ($result) {
      _infusionsoft_devel('action', 'Removed follow up sequence %followup_id from %name in infusionsoft / uid=%uid.', array('%followup_id' => $context['campaign'], '%name' => check_plain($user->name), '%uid' => $user->uid));
    }
  }
}


/**
 * Remove Infusionsoft Campaign from User
 *
 *  Implementation of Drupal action form.
 *  Reference: http://www.sysarchitects.com/node/47
 *
 * A drupal form which allows you to select an infusionsoft Campaign
 *
 */
function infusionsoft_remove_campaign_from_user_action_form($context) {
  $form['campaign'] = array(
    '#title' => t('Follow up sequence'),
    '#type' => 'select',
    '#description' => t('Choose an infusionsoft follow up sequence you would like to remove from a user'),
    '#options' => _infusionsoft_get_follow_up_select(),
    '#default_value' => isset($context['campaign']) ? $context['campaign'] : '',
  );
  $form['#validate'][] = 'infusionsoft_remove_campaign_from_user_action_validate';
  $form['#submit'][] = 'infusionsoft_remove_campaign_from_user_action_submit';
  return $form;
}

function infusionsoft_remove_campaign_from_user_action_validate($form, $form_state) {
  $numeric = is_numeric($form_state['values']['campaign']);
  if (!$numeric) {
    form_set_error('campaign', 'invalid option');
  }
}

function infusionsoft_remove_campaign_from_user_action_submit($form, $form_state) {
  return array('campaign' => $form_state['values']['campaign']);
}

/**
 * Pause Infusionsoft Campaign from User
 *
 * Implementation of a Drupal action.
 * Adds an infusionsoft contact group to the current user.
 *
 */
function infusionsoft_pause_campaign_for_user_action(&$object, $context = array()) {
  // Get the user object.
  if (isset($object->uid)) {
    $user = user_load($object->uid);
  }
  elseif (isset($context['uid'])) {
    $user = user_load($context['uid']);
  }
  else {
    global $user;
  }
  // Make sure we have a user record.
  if (!empty($user->uid)) {
    // assign campaign
    $contact_id = infusionsoft_contactid_by_user($user);
    $result = infusionsoft_campaign_update($contact_id, $context['campaign'], 'pause');
    // record a message noting the action taken
    if ($result) {
      _infusionsoft_devel('action', 'Paused follow up sequence %followup_id from %name in infusionsoft / uid=%uid.', array('%followup_id' => $context['campaign'], '%name' => check_plain($user->name), '%uid' => $user->uid));
    }
  }
}


/**
 * Pause Infusionsoft Campaign from User
 *
 *  Implementation of Drupal action form.
 *  Reference: http://www.sysarchitects.com/node/47
 *
 * A drupal form which allows you to select an infusionsoft Campaign
 *
 */
function infusionsoft_pause_campaign_for_user_action_form($context) {
  $form['campaign'] = array(
    '#title' => t('Follow up sequence'),
    '#type' => 'select',
    '#description' => t('Select an infusionsoft follow up sequence you would like to pause for a user'),
    '#options' => _infusionsoft_get_follow_up_select(),
    '#default_value' => isset($context['campaign']) ? $context['campaign'] : '',
  );
  $form['#validate'][] = 'infusionsoft_pause_campaign_for_user_action_validate';
  $form['#submit'][] = 'infusionsoft_pause_campaign_for_user_action_submit';
  return $form;
}

function infusionsoft_pause_campaign_for_user_action_validate($form, $form_state) {
  $numeric = is_numeric($form_state['values']['campaign']);
  if (!$numeric) {
    form_set_error('campaign', 'invalid option');
  }
}

function infusionsoft_pause_campaign_for_user_action_submit($form, $form_state) {
  return array('campaign' => $form_state['values']['campaign']);
}

/**
 * Resume Infusionsoft Campaign from User
 *
 * Implementation of a Drupal action.
 * Adds an infusionsoft contact group to the current user.
 *
 */
function infusionsoft_resume_campaign_for_user_action(&$object, $context = array()) {
  // Get the user object.
  if (isset($object->uid)) {
    $user = user_load($object->uid);
  }
  elseif (isset($context['uid'])) {
    $user = user_load($context['uid']);
  }
  else {
    global $user;
  }
  // Make sure we have a user record.
  if (!empty($user->uid)) {
    // assign campaign
    $contact_id = infusionsoft_contactid_by_user($user);
    $result = infusionsoft_campaign_update($contact_id, $context['campaign'], 'resume');
    // record a message noting the action taken
    if ($result) {
      _infusionsoft_devel('action', 'Resumed follow up sequence %followup_id for %name in infusionsoft / uid=%uid.', array('%followup_id' => $context['campaign'], '%name' => check_plain($user->name), '%uid' => $user->uid));
    }
  }
}


/**
 * Resume Infusionsoft Campaign from User
 *
 *  Implementation of Drupal action form.
 *  Reference: http://www.sysarchitects.com/node/47
 *
 * A drupal form which allows you to select an infusionsoft Campaign
 *
 */
function infusionsoft_resume_campaign_for_user_action_form($context) {
  $form['campaign'] = array(
    '#title' => t('Follow up sequence'),
    '#type' => 'select',
    '#description' => t('Select an infusionsoft follow up sequence you would like to resume for a user'),
    '#options' => _infusionsoft_get_follow_up_select(),
    '#default_value' => isset($context['campaign']) ? $context['campaign'] : '',
  );
  $form['#validate'][] = 'infusionsoft_resume_campaign_for_user_action_validate';
  $form['#submit'][] = 'infusionsoft_resume_campaign_for_user_action_submit';
  return $form;
}

function infusionsoft_resume_campaign_for_user_action_validate($form, $form_state) {
  $numeric = is_numeric($form_state['values']['campaign']);
  if (!$numeric) {
    form_set_error('campaign', 'invalid option');
  }
}

function infusionsoft_resume_campaign_for_user_action_submit($form, $form_state) {
  return array('campaign' => $form_state['values']['campaign']);
}

/**
 * Run Infusionsoft ActionSet on User
 *
 * Implementation of a Drupal action.
 * Adds an infusionsoft contact group to the current user.
 *
 */
function infusionsoft_run_action_set_action(&$object, $context = array()) {
  // Get the user object.
  if (isset($object->uid)) {
    $user = user_load($object->uid);
  }
  elseif (isset($context['uid'])) {
    $user = user_load($context['uid']);
  }
  else {
    global $user;
  }
  // Make sure we have a user record.
  if (!empty($user->uid)) {
    // assign campaign
    $contact_id = infusionsoft_contactid_by_user($user);
    $result = infusionsoft_run_actionset($contact_id, $context['action_set']);
    // record a message noting the action taken
    if ($result) {
      _infusionsoft_devel('action', 'Ran Infusionsoft action set %action_set_id from %name in infusionsoft / uid=%uid.', array('%action_set_id' => $context['campaign'], '%name' => check_plain($user->name), '%uid' => $user->uid));
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
function infusionsoft_run_action_set_action_form($context) {
  $form['action_set'] = array(
    '#title' => t('Action Set'),
    '#type' => 'select',
    '#description' => t('Choose an infusionsoft action set you would like to run for a user'),
    '#options' => _infusionsoft_get_action_set_select(),
    '#default_value' => isset($context['action_set']) ? $context['action_set'] : '',
  );
  $form['#validate'][] = 'infusionsoft_run_action_set_action_validate';
  $form['#submit'][] = 'infusionsoft_run_action_set_action_submit';
  return $form;
}

function infusionsoft_run_action_set_action_validate($form, $form_state) {
  $numeric = is_numeric($form_state['values']['action_set']);
  if (!$numeric) {
    form_set_error('action_set', 'invalid option');
  }
}

function infusionsoft_run_action_set_action_submit($form, $form_state) {
  return array('action_set' => $form_state['values']['action_set']);
}
