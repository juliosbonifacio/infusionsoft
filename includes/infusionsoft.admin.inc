<?php
/**
 * @file
 * Settings for infusionsoft
 */

/**
 * Settings page for infusionsoft module
 */
function infusionsoft_admin_settings_page() {
  return drupal_get_form('infusionsoft_admin_settings_form');
}

/**
 * Implements hook_form().
 */
function infusionsoft_admin_settings_form() {
  $api_key = variable_get('infusionsoft_apikey', '');
  if (!empty($api_key)) {
    drupal_set_message(t('You have an infusionsoft API key on file'));
  }
  $form = array();
  $form['required'] = array(
    '#type' => 'fieldset',
    '#title' => t('Required'),
  );
  // Store the Infusionsoft application name, and API key, in the Drupal
  // variables table.
  $form['required']['infusionsoft_appname'] = array(
    '#type' => 'textfield',
    '#title' => t('Application Name'),
    '#default_value' => variable_get('infusionsoft_appname', ''),
    '#description' => t('Your Infusionsoft application name: xxx in http://xxx.infusionsoft.com'),
    '#required' => TRUE,
    );
  // Keep this somewhat secure by not exposing the API key in the interface.
  if (empty($api_key)) {
    $form['required']['infusionsoft_apikey'] = array(
      '#type' => 'textfield',
      '#title' => t('API Key'),
      '#default_value' => variable_get('infusionsoft_apikey', ''),
      '#description' => t('Copy and paste your Infusionsoft API key -- see README for instructions on how to find this.'),
      '#required' => TRUE,
    );
    $form['developer'] = array(
      '#type' => 'fieldset',
      '#title' => t('Developer'),
    );
  }
  else {
    $form['developer'] = array(
      '#type' => 'fieldset',
      '#title' => t('Developer'),
    );
    $form['developer']['infusionsoft_apikey_reset'] = array(
      '#type' => 'checkbox',
      '#title' => t('Reset API Key'),
      '#default_value' => FALSE,
      '#description' => t('Check to reset the api key you have on file.'),
    );
  }
  $form['developer']['infusionsoft_user_cache_reset'] = array(
      '#type' => 'checkbox',
      '#title' => t('Clear drupal user to infusionsoft user cache.'),
      '#default_value' => 0,
  );
  $form['tracking'] = array(
    '#type' => 'fieldset',
    '#title' => t('Web Analytics and tracking'),
  );
  $form['tracking']['infusionsoft_tracking'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add infusionsoft tracking code.'),
    '#default_value' => variable_get('infusionsoft_tracking', TRUE),
    '#description' => t('Will add the js tracking code to your website.'),
  );
  $form['logging'] = array(
    '#type' => 'fieldset',
    '#title' => t('Logging'),
  );
  $form['logging']['infusionsoft_watchdog'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable watchdog logging'),
    '#default_value' => variable_get('infusionsoft_watchdog', FALSE),
    '#description' => t('You will see logging inside watchdog'),
  );
  if (module_exists('devel')) {
    $form['logging']['infusionsoft_devel'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable devel logging'),
      '#default_value' => variable_get('infusionsoft_devel', FALSE),
      '#description' => t('Yot will see devel messages on your site'),
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#weight' => 100,
  );
  return $form;
}

/**
 * Validation callback for infusionsoft_admin_settings_form.
 */
function infusionsoft_admin_settings_form_validate($form, &$form_state) {
  // validate appname
  $app_name = $form_state['values']['infusionsoft_appname'];
  if (empty($app_name) || $app_name != check_plain($app_name)) {
    form_set_error('infusionsoft_appname', t('Your Application Name  is not valid.'));
  }
  // we only need to validate if there is no api key stored
  $stored_api_key = variable_get('infusionsoft_apikey', '');
  if (empty($stored_api_key)) {
    $new_api_key = $form_state['values']['infusionsoft_apikey'];
    // something weird is happening, lets display an error
    if (empty($new_api_key) || $new_api_key != check_plain($new_api_key)) {
      form_set_error('infusionsoft_apikey', t('Your api key is not valid.'));
    }
  }
}

/**
 * Process infusionsoft_admin_settings_form form submissions.
 */
function infusionsoft_admin_settings_form_submit($form, &$form_state) {
  $app_name = $form_state['values']['infusionsoft_appname'];
  if ($app_name != '') {
    variable_set('infusionsoft_appname', check_plain($app_name));
  }
  if ($form_state['values']['infusionsoft_apikey_reset']) {
    variable_set('infusionsoft_apikey', '');
  }
  if (!empty($form_state['values']['infusionsoft_apikey'])) {
    $api_key = $form_state['values']['infusionsoft_apikey'];
    if ($app_name != '') {
      variable_set('infusionsoft_apikey', check_plain($api_key));
    }
  }
  switch ($form_state['values']['infusionsoft_tracking']) {
    case TRUE:
      variable_set('infusionsoft_tracking', TRUE);
      _infusionsoft_update_tracking_code();
      break;

    case FALSE:
      variable_set('infusionsoft_tracking', FALSE);
      break;

  }
  switch ($form_state['values']['infusionsoft_watchdog']) {
    case TRUE:
      variable_set('infusionsoft_watchdog', TRUE);
      break;

    case FALSE:
      variable_set('infusionsoft_watchdog', FALSE);
      break;

  }
  if (isset($form_state['values']['infusionsoft_devel'])) {
    switch ($form_state['values']['infusionsoft_devel']) {
      case TRUE:
        variable_set('infusionsoft_devel', TRUE);
        break;

      case FALSE:
        variable_set('infusionsoft_devel', FALSE);
        break;

    }
  }

  $reset = $form_state['values']['infusionsoft_user_cache_reset'];
  if ($reset != 0) {
    _infusionsoft_user_cache_clear();
    drupal_set_message(t('Infusionsoft user cache cleared'));
  }
}

/**
 * Page callback to list all the custom fields.
 */
function infusionsoft_admin_custom_contact_fields() {
  $output = t('A list of the custom contact fields currently in your database.');
  if (variable_get('infusionsoft_apikey', '') == '') {
    drupal_set_message(t('You must have a valid api key to see your custom fields'));
    return $output;
  }
  $selected_fields = array('Label', 'Name', 'Values', 'GroupId');
  $results = infusionsoft_send_request('DataService.query', 'DataFormField', 1000, 0, array('FormId' => -1), $selected_fields);
  $groups = infusionsoft_send_request('DataService.query', 'DataFormGroup', 1000, 0, array('Id' => '%'), array('Id', 'Name'));

  if (!empty($results) && !empty($groups)) {
    $rows = array();
    $group_names = array();
    $custom_fields = array();

    foreach ($groups as $group) {
      $group_names[$group['Id']] = $group['Name'];
    }
    foreach ($results as $result) {
      $rows[] = array(
        $result['Label'],
        '_' . $result['Name'],
        $result['Values'],
        $group_names[$result['GroupId']],
      );
      $custom_fields[] = '_' . $result['Name'];
    }
    array_pop($selected_fields);
    $selected_fields[] = 'Group';
    $output .= theme_table($selected_fields, $rows);
  }
  else {
    $output .= t('Unable to find custom fields.');
  }

  global $user;
  $contact_id = infusionsoft_contactid_by_user($user);
  if ($contact_id) {
    $custom_results = infusionsoft_send_request('ContactService.load', $contact_id, $custom_fields);
    $output .= '<h2>' . t('Results for currently logged in user.') . '</h2>';
    foreach($custom_results as $field_name => $value) {
      $custom_rows[] = array(
        $field_name,
        $value,
      );
    }
    $output .= theme_table(array('Custom field', 'Result'), $custom_rows);
  }

  return $output;
}
